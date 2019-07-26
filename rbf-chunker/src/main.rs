use std::fs::File;
use std::io::prelude::*;
use std::io::{Error, ErrorKind};
use std::path::{Path, PathBuf};

#[derive(Debug, Copy, Clone)]
enum SectionType {
    Unkown,
    Header,
    Footer,
    Packet { crc: [u8; 2] },
    PacketCRC,
}

#[derive(Debug, Copy, Clone)]
struct SectionInfo {
    offset: usize,
    size: usize,
    stype: SectionType,
}

impl SectionInfo {
    fn new_header(size: usize) -> Self {
        SectionInfo {
            offset: 0,
            size,
            stype: SectionType::Header,
        }
    }

    fn new_unkown(offset: usize, size: usize) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Unkown,
        }
    }

    fn new_footer(offset: usize, size: usize) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Footer,
        }
    }

    fn new_packet(offset: usize, size: usize, crc: [u8; 2]) -> Self {
        SectionInfo {
            offset,
            size,
            stype: SectionType::Packet { crc },
        }
    }

    fn new_packet_crc(offset: usize) -> Self {
        SectionInfo {
            offset,
            size: 2,
            stype: SectionType::PacketCRC,
        }
    }
}

const RBF_SIZE: usize = 7020944;
const PACKET_MIN_SIZE: usize = 916;
const PACKET_MAX_SIZE: usize = 916;
const DISCARD_MAX_SIZE: usize = PACKET_MAX_SIZE;
const HEADER_SIZE: usize = 0x84;
const FOOTER_SIZE: usize = 0x268;
const EXPECTED_UNKOWN_SIZE: usize = 0x378; // Used only for optimization.

fn partition_rbf(
    id: PathBuf,
    rbf: &[u8],
) -> Result<Vec<SectionInfo>, (Vec<SectionInfo>, &'static str)> {
    let mut section_infos = Vec::with_capacity(
        (RBF_SIZE - EXPECTED_UNKOWN_SIZE - HEADER_SIZE - FOOTER_SIZE) / PACKET_MIN_SIZE * 2 + 3,
    );
    section_infos.push(SectionInfo::new_header(HEADER_SIZE));

    let mut discard_start = HEADER_SIZE;
    let mut section_start = HEADER_SIZE;
    let mut section_end = HEADER_SIZE;
    let mut running_crc = crc16::State::<crc16::MODBUS>::new();

    while section_end != rbf.len() {
        if section_start == section_end {
            section_end = section_end + PACKET_MIN_SIZE;
            if section_end > rbf.len() {
                // footer found
                break;
            }
            running_crc.update(&rbf[section_start..section_end - 2]);
        } else if section_end - section_start < PACKET_MAX_SIZE {
            section_end = section_end + 1;
            running_crc.update(&[rbf[section_end - 1 - 2]]);
        } else if section_start - discard_start >= DISCARD_MAX_SIZE {
            section_infos.push(SectionInfo::new_unkown(
                discard_start,
                section_start - discard_start,
            ));
            return Err((section_infos, "No packet found. Refuse to continue."));
        } else {
            running_crc = crc16::State::<crc16::MODBUS>::new();
            section_start = section_start + 1;
            section_end = section_start;
            continue;
        }

        let crc = running_crc.get();
        let crc = [(crc & 0xFF) as u8, (crc >> 8) as u8];
        let target_crc = [rbf[section_end - 2], rbf[section_end - 1]];

        if crc == target_crc && crc != [0, 0] {
            if section_start != discard_start {
                section_infos.push(SectionInfo::new_unkown(
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
        eprintln!("{:?}: Discarding last packet as was footer.", id);
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

    Ok(section_infos)
}

fn dump_sections(target_dir: &PathBuf, sections: &[SectionInfo]) -> std::io::Result<()> {
    let mut file = File::create(target_dir.join("section_infos"))?;
    for section in sections {
        writeln!(&mut file, "{:?}", section)?;
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

    for filename in files {
        let filepath: &Path = filename.as_ref();
        let target_dir = target_dir.join(filepath.file_name().unwrap());
        filepath
            .canonicalize()
            .and_then(|filepath| {
                println!("{:?}: Proccessing into {:?}", filepath, target_dir);

                if target_dir.exists() {
                    Err(Error::new(
                        ErrorKind::AlreadyExists,
                        "Target dir already exists. Refuse to overwrite.",
                    ))?
                }
                std::fs::create_dir(&target_dir)?;

                let mut file = File::open(&filename)?;
                let mut rbf = vec![0; RBF_SIZE];
                file.read_exact(&mut rbf[..])?;

                let sections = partition_rbf(filepath, &rbf[..]).map_err(|(s, e)| {
                    dump_sections(&target_dir, &s).unwrap();
                    Error::new(ErrorKind::Other, e)
                })?;

                dump_sections(&target_dir, &sections)?;

                for (i, section) in sections.iter().enumerate() {
                    File::create(target_dir.join(
                        i.to_string()
                            + match section.stype {
                                SectionType::Unkown => "unkown",
                                SectionType::Header => "header",
                                SectionType::Footer => "footer",
                                SectionType::Packet { .. } => "packet",
                                SectionType::PacketCRC => "packet_crc",
                            },
                    ))?
                    .write_all(&rbf[section.offset..section.offset + section.size])?;
                }

                Ok(())
            })
            .unwrap_or_else(|err| eprintln!("{:?}: Failed to proccess with {:?}", filename, err));
    }
}
