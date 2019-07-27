#![feature(maybe_uninit_slice, maybe_uninit_array)]

#[macro_use]
extern crate core;

mod int_writer;

use int_writer::WriteInt;
use rayon::prelude::*;
use std::fmt::{self, Display, Formatter};
use std::fs::File;
use std::io::prelude::*;
use std::io::{Error, ErrorKind};
use std::path::{Path, PathBuf};

#[derive(Copy, Clone)]
enum SectionType {
    Unknown,
    Header,
    Footer,
    Packet { crc: [u8; 2] },
    PacketCRC,
}

impl Display for SectionType {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            SectionType::Unknown => "Unknown",
            SectionType::Header => "Header",
            SectionType::Footer => "Footer",
            SectionType::Packet { .. } => "Packet",
            SectionType::PacketCRC => "PacketCRC",
        })
    }
}

#[derive(Copy, Clone)]
struct SectionInfo {
    offset: usize,
    size: usize,
    stype: SectionType,
}

impl SectionInfo {
    #[inline]
    fn new_header(size: usize) -> Self {
        SectionInfo {
            offset: 0,
            size,
            stype: SectionType::Header,
        }
    }

    #[inline]
    fn new_unknown(offset: usize, size: usize) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Unknown,
        }
    }

    #[inline]
    fn new_footer(offset: usize, size: usize) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Footer,
        }
    }

    #[inline]
    fn new_packet(offset: usize, size: usize, crc: [u8; 2]) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Packet { crc },
        }
    }

    #[inline]
    fn new_packet_crc(offset: usize) -> Self {
        SectionInfo {
            offset,
            size: 2,
            stype: SectionType::PacketCRC,
        }
    }
}

const PACKET_MIN_SIZE: usize = 916;
const PACKET_MAX_SIZE: usize = 916;
const DISCARD_MAX_SIZE: usize = PACKET_MAX_SIZE;
const HEADER_SIZE: usize = 0x84;
const FOOTER_SIZE: usize = 0x268;
const EXPECTED_UNKNOWN_SIZE: usize = 0x378; // Used only for optimization.

#[inline]
fn partition_rbf(id: PathBuf, rbf: &[u8]) -> Vec<SectionInfo> {
    let mut section_infos = Vec::with_capacity(
        (rbf.len() - EXPECTED_UNKNOWN_SIZE - HEADER_SIZE - FOOTER_SIZE) / PACKET_MIN_SIZE * 2 + 3,
    );
    section_infos.push(SectionInfo::new_header(HEADER_SIZE));

    let mut discard_start = HEADER_SIZE;
    let mut section_start = HEADER_SIZE;
    let mut section_end = HEADER_SIZE;
    let mut running_crc = crc16::State::<crc16::MODBUS>::new();

    while section_end != rbf.len() {
        if section_start == section_end {
            section_end += PACKET_MIN_SIZE;
            if section_end > rbf.len() {
                // footer found
                break;
            }
            running_crc.update(&rbf[section_start..section_end - 2]);
        } else if section_end - section_start < PACKET_MAX_SIZE {
            section_end += 1;
            running_crc.update(&[rbf[section_end - 1 - 2]]);
        } else if section_start - discard_start >= DISCARD_MAX_SIZE {
            panic!("Failed to find sections starting at {:?}", discard_start);
        } else {
            running_crc = crc16::State::<crc16::MODBUS>::new();
            section_start += 1;
            section_end = section_start;
            continue;
        }

        let crc = running_crc.get();
        let crc = [(crc & 0xFF) as u8, (crc >> 8) as u8];
        let target_crc = [rbf[section_end - 2], rbf[section_end - 1]];

        if crc == target_crc && crc != [0, 0] {
            if section_start != discard_start {
                section_infos.push(SectionInfo::new_unknown(
                    discard_start,
                    section_start - discard_start,
                ));
            }
            section_infos.push(SectionInfo::new_packet(
                section_start,
                section_end - section_start - 2,
                crc,
            ));
            section_infos.push(SectionInfo::new_packet_crc(section_end - 2));

            section_start = section_end;
            discard_start = section_end;
            running_crc = crc16::State::<crc16::MODBUS>::new();
        }
    }

    let (offset, size) = if discard_start == rbf.len() {
        // Last packet was actually the footer.
        // Discard it.
        //
        // Wtf???
        eprintln!("{:?}: Discarding the last packet as it was the footer.", id);
        let last_packet_crc = section_infos.pop().unwrap();
        let last_packet = section_infos.pop().unwrap();

        match (last_packet_crc.stype, last_packet.stype) {
            (SectionType::PacketCRC, SectionType::Packet { .. }) => (),
            _ => panic!(),
        }

        (last_packet.offset, last_packet.size + 2)
    } else {
        (discard_start, rbf.len() - discard_start)
    };

    assert_eq!(size, FOOTER_SIZE);
    section_infos.push(SectionInfo::new_footer(offset, size));

    section_infos
}

#[inline]
fn dump_sections(target_dir: &Path, sections: &[SectionInfo]) -> std::io::Result<()> {
    // Doing all this writing here, instead of using rust's debug trait provides
    // a 2x-3x speed up.
    //
    // Presumably, rustc fails to inline/optimize the stdlib, so moving it here
    // makes stuff faster... somehow?
    //
    // Since we don't need to output the exact thing Debug normally outputs, we
    // can save another 20%.
    let mut f = File::create(target_dir.join("section_infos"))?;
    for section in sections {
        usize::write_int(section.offset, &mut f)?;
        f.write_all(&[b','])?;
        usize::write_int(section.size, &mut f)?;
        write!(f, ",{},", section.stype)?;
        match section.stype {
            SectionType::Packet { crc } => {
                u8::write_int(crc[0], &mut f)?;
                f.write_all(&[b' '])?;
                u8::write_int(crc[1], &mut f)?;
            }
            _ => (),
        }
        f.write_all(&[b'\n'])?;
    }
    Ok(())
}

fn main() {
    let mut args = std::env::args_os().skip(1);

    let target_dir = args.next().unwrap();
    let target_dir: &Path = target_dir.as_ref();
    let target_dir = target_dir.canonicalize().unwrap();

    let mut files: Vec<_> = args.collect();
    let num_files = files.len();
    files.sort_unstable();
    files.dedup();
    if files.len() != num_files {
        eprintln!("Duplicate files found, ignoring.");
    }

    files.par_iter().for_each(|filename| {
        let filepath: &Path = filename.as_ref();
        let target_dir = target_dir.join(filepath.file_name().unwrap());
        filepath
            .canonicalize()
            .and_then(|filepath| {
                println!("{:?}: Processing into {:?}", filepath, target_dir);

                if target_dir.exists() {
                    Err(Error::new(
                        ErrorKind::AlreadyExists,
                        "Target dir already exists. Refuse to overwrite.",
                    ))?
                }
                std::fs::create_dir(&target_dir)?;

                let mut file = File::open(&filename)?;
                let mut rbf = vec![0; file.metadata()?.len() as usize];
                file.read_exact(&mut rbf[..])?;

                let sections = partition_rbf(filepath, &rbf[..]);

                dump_sections(&target_dir, &sections)?;

                for (i, section) in sections.iter().enumerate() {
                    File::create(target_dir.join(format!("{} {}", i, section.stype)))?
                        .write_all(&rbf[section.offset..section.offset + section.size])?;
                }

                Ok(())
            })
            .unwrap_or_else(|err| eprintln!("{:?}: Failed to process with {:?}", filename, err));
    })
}
