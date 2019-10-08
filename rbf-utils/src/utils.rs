// Not all functions are used.
#![allow(dead_code, unused_macros)]

use rayon::prelude::*;
use std::borrow::Borrow;
use std::fmt::{self, Debug, Display, Formatter};
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::path::Path;

macro_rules! make_io_error {
    ($err:expr $(, $($arg:tt)*)?) => (Err(Error::new(ErrorKind::Other, format!($err, $($($arg)*)? ) )));
}

#[derive(Copy, Clone)]
pub enum SectionType {
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
pub struct SectionInfo {
    pub offset: usize,
    pub size: usize,
    pub stype: SectionType,
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

pub const PACKET_MIN_SIZE: usize = 524; // 916
pub const PACKET_MAX_SIZE: usize = 524; // 916
const DISCARD_MAX_SIZE: usize = PACKET_MAX_SIZE;
const HEADER_SIZE: usize = 0x84;

// Used only for optimization. Underguessing is better than over guessing.
const FOOTER_SIZE: usize = 0x1f8; // 0x268
const EXPECTED_UNKNOWN_SIZE: usize = 0x1f0; // 0x378

#[inline]
pub fn partition_rbf(id: &str, rbf: &[u8]) -> Vec<SectionInfo> {
    let mut section_infos = Vec::with_capacity(
        (rbf.len() - EXPECTED_UNKNOWN_SIZE - HEADER_SIZE - FOOTER_SIZE)
            / PACKET_MIN_SIZE
            * 2
            + 3,
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
            // Often, it's just a mispicked header size.
            panic!("Failed to find sections starting at {:x?}", discard_start);
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

    section_infos.push(SectionInfo::new_footer(offset, size));

    section_infos
}

#[inline]
pub fn dump_sections(
    target_dir: &Path,
    sections: &[SectionInfo],
) -> io::Result<()> {
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
        write!(
            f,
            "{:x},{:x},{}",
            section.offset, section.size, section.stype
        )?;
        match section.stype {
            SectionType::Packet { crc } => {
                write!(f, ",{:x} {:x}", crc[0], crc[1])?;
            }
            _ => (),
        }
        write!(f, "\n")?;
    }
    Ok(())
}

#[inline]
pub fn load_rbf(filepath: &Path) -> io::Result<Vec<u8>> {
    let mut file = File::open(filepath)?;
    let mut rbf = vec![0; file.metadata()?.len() as usize];
    file.read_exact(&mut rbf[..])?;
    Ok(rbf)
}

#[inline]
pub fn find_diff_bytes<'a, T1, T2>(
    files: &'a mut Vec<(T1, T2)>,
    master_file: (T1, T2),
    bytes_per_line: usize,
) -> Vec<(usize, usize, Vec<u8>)>
where
    [(T1, T2)]: IntoParallelRefIterator<'a, Item = &'a (T1, T2)>,
    T1: Borrow<[u8]> + Send + Sync,
    T2: Debug + Send + Sync,
{
    // Pass one: We track down which bytes vary between the master file and the
    // others. We can then merge these vectors to get all the bytes to examine.
    //
    // Consider the master and two files, there are only three possible
    // results:
    // MS | F1 | F2
    // 00 | 00 | 00 -> No diffs
    // 00 | 00 | 01 -> One diff in F2's diff list.
    // 01 | 00 | 00 -> One diff in both F1's and F2's diff list.
    //
    // If we merge then dedupe the diff lists, we will have an list of
    // bytes that vary between at least two files.

    eprintln!("Diffing {} files...", files.len());

    let mut diff_bytes: Vec<_> = files
        .par_iter()
        .map(|(rbf, id)| {
            eprintln!("Pass 1: {:?}", id);

            rbf.borrow()
                .par_iter()
                .cloned()
                .zip(master_file.0.borrow().par_iter().cloned())
                .enumerate()
                .filter_map(
                    |(addr, (b1, b2))| if b1 != b2 { Some(addr) } else { None },
                )
        })
        .flatten()
        .collect();
    diff_bytes.par_sort_unstable();
    diff_bytes.dedup();

    files.push(master_file);

    // Pass 2: We collect the actual differences.
    eprintln!("Gathering {} bytes...", diff_bytes.len());

    let diff_bytes: Vec<_> = diff_bytes
        .into_par_iter()
        .map(|addr| {
            let line = addr / bytes_per_line;
            let values_per_file: Vec<_> =
                files.iter().map(|(rbf, _)| rbf.borrow()[addr]).collect();
            (addr, line, values_per_file)
        })
        .collect();

    diff_bytes
}
