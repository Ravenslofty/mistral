#![feature(const_generics)]

#[macro_use]
extern crate arrayref;

use std::fs::File;
use std::io::prelude::*;
use std::io::{Error, ErrorKind};
use std::path::{Path, PathBuf};

mod array_gen;
use array_gen::ArrayGen;

#[derive(Debug, Copy, Clone)]
struct CRCInfo {
    packet_number: u16,
    target_crc: [u8; 2],
    crc: [u8; 2],
}

fn handle_file(target_dir: PathBuf, filename: PathBuf) -> std::io::Result<()> {
    const PACKET_MAX_SIZE: usize = 1024 * 10;
    const PACKET_MIN_SIZE: usize = 600;
    const HEADER_SIZE: usize = 0x84;

    if target_dir.exists() {
        Err(Error::new(
            ErrorKind::AlreadyExists,
            "Target dir already exists. Refuse to overwrite.",
        ))?
    }

    std::fs::create_dir(target_dir.clone())?;
    let mut file = File::open(filename.clone())?;
    let mut working_buffer: ArrayGen<u8, { PACKET_MAX_SIZE }> = [0; PACKET_MAX_SIZE].into();

    // Read the header.
    file.read_exact(&mut working_buffer[..HEADER_SIZE])?;
    File::create(target_dir.clone().join("header"))?.write_all(&working_buffer[..HEADER_SIZE])?;

    let mut crc_infos = vec![];

    let mut read_so_far = 0;
    let mut packet_number = 0u16;
    loop {
        match file.read(&mut working_buffer[read_so_far..]) {
            Ok(0) => {
                if read_so_far == 0 {
                    // Last packet was actually the footer.
                    // Discard it.
                    //
                    // Wtf???
                    eprintln!("{:?}: Discarding last packet as was footer.", filename);

                    assert!(packet_number > 0);
                    let filename = format!("packet{:05}", packet_number - 1);
                    std::fs::remove_file(target_dir.clone().join(filename))?;
                    crc_infos.pop().unwrap();
                }
                File::create(target_dir.clone().join("footer"))?
                    .write_all(&working_buffer[..read_so_far])?;

                let mut crc_info_file = File::create(target_dir.clone().join("crcinfo"))?;
                for crc_info in crc_infos {
                    writeln!(&mut crc_info_file, "{:?}", crc_info)?;
                }
                return Ok(());
            }
            Ok(n) => {
                let old_read_so_far = read_so_far;
                read_so_far = read_so_far + n;
                assert!(read_so_far <= PACKET_MAX_SIZE);

                'try_again: loop {
                    for end_read_so_far in (old_read_so_far + 1)..=read_so_far {
                        if end_read_so_far < PACKET_MIN_SIZE { continue; }
                        for start_read_so_far in (0..end_read_so_far).rev() {
                            if end_read_so_far - start_read_so_far < PACKET_MIN_SIZE { continue; }
                            let crc_info = crc_packet(packet_number, &working_buffer[start_read_so_far..end_read_so_far]);
                            if crc_info.crc == crc_info.target_crc {
                                let filename = format!("packet{:05}", packet_number);
                                File::create(target_dir.clone().join(filename))?
                                    .write_all(&working_buffer[start_read_so_far..end_read_so_far])?;

                                if start_read_so_far != 0 {
                                    let filename = format!("packet{:05}-prefix", packet_number);
                                    File::create(target_dir.clone().join(filename))?
                                        .write_all(&working_buffer[0..start_read_so_far])?;
                                }
                                crc_infos.push(crc_info);
                                if read_so_far != end_read_so_far {
                                    assert!(read_so_far > end_read_so_far);
                                    assert!(read_so_far <= PACKET_MAX_SIZE);
                                    unsafe {
                                        std::ptr::copy(
                                            &working_buffer[end_read_so_far] as *const u8,
                                            &mut working_buffer[0] as *mut u8,
                                            read_so_far - end_read_so_far,
                                        );
                                    }
                                }
                                packet_number = packet_number + 1;
                                read_so_far = 0;
                                continue 'try_again;
                            }
                        }
                    }
                    break;
                }

                if read_so_far == PACKET_MAX_SIZE {
                    Err(Error::new(
                        ErrorKind::InvalidData,
                        "No packet found. Refuse to continue.",
                    ))?
                }
            }
            Err(ref err) if err.kind() == ErrorKind::Interrupted => (),
            err => return err.map(|_| unreachable!()),
        }
    }
}

fn crc_packet(packet_number: u16, packet: &[u8]) -> CRCInfo {
    let crc = crc16::State::<crc16::MODBUS>::calculate(
        &packet[..packet.len() - 2],
    );
    CRCInfo {
        packet_number,
        target_crc: array_ref!(packet, packet.len() - 2, 2).clone(),
        crc: [(crc & 0xFF) as u8, ((crc & !0xFF) >> 8) as u8],
    }
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
                handle_file(target_dir, filepath)
            })
            .unwrap_or_else(|err| eprintln!("{:?}: Failed to proccess with {:?}", filename, err));
    }
}
