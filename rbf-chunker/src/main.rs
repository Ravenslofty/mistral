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
struct PacketInfo {
    packet_number: u16,
    packet_offset: usize,
    packet_size: usize,
    discards: usize,
    target_crc: [u8; 2],
    crc: [u8; 2],
}

impl PacketInfo {
    fn new(
        packet_number: u16,
        packet_offset: usize,
        discards: usize,
        packet: &[u8],
        crc: u16,
    ) -> Self {
        PacketInfo {
            packet_number,
            packet_offset,
            discards,
            packet_size: packet.len(),
            target_crc: array_ref!(packet, packet.len() - 2, 2).clone(),
            crc: [(crc & 0xFF) as u8, ((crc & !0xFF) >> 8) as u8],
        }
    }
}

fn dump_packet_infos(target_dir: &PathBuf, packet_infos: &[PacketInfo]) -> std::io::Result<()> {
    let mut crc_info_file = File::create(target_dir.join("packet_infos"))?;
    for crc_info in packet_infos {
        writeln!(&mut crc_info_file, "{:?}", crc_info)?;
    }
    Ok(())
}

fn handle_file(target_dir: PathBuf, filename: PathBuf) -> std::io::Result<()> {
    const PACKET_MIN_SIZE: usize = 300;
    const PACKET_MAX_SIZE: usize = 1024 * 2;
    const DISCARD_MAX_SIZE: usize = 1024 * 5;
    const HEADER_SIZE: usize = 0x84;

    if target_dir.exists() {
        Err(Error::new(
            ErrorKind::AlreadyExists,
            "Target dir already exists. Refuse to overwrite.",
        ))?
    }

    std::fs::create_dir(target_dir.clone())?;
    let mut file = File::open(filename.clone())?;
    let mut working_buffer: ArrayGen<u8, { PACKET_MAX_SIZE + DISCARD_MAX_SIZE }> =
        [0; PACKET_MAX_SIZE + DISCARD_MAX_SIZE].into();

    // Read the header.
    file.read_exact(&mut working_buffer[..HEADER_SIZE])?;
    File::create(target_dir.join("header"))?.write_all(&working_buffer[..HEADER_SIZE])?;

    let mut running_crc = crc16::State::<crc16::MODBUS>::new();
    let mut packet_infos = vec![];

    let mut read_so_far = 0;
    let mut discard_so_far = 0;
    let mut packet_offset = HEADER_SIZE;
    let mut packet_number = 0u16;
    loop {
        match file.read(
            &mut working_buffer[discard_so_far + read_so_far..discard_so_far + PACKET_MAX_SIZE],
        ) {
            Ok(0) => {
                if read_so_far == 0 {
                    // Last packet was actually the footer.
                    // Discard it.
                    //
                    // Wtf???
                    eprintln!("{:?}: Discarding last packet as was footer.", filename);

                    assert!(packet_number > 0);
                    let filename = format!("packet{:05}", packet_number - 1);
                    std::fs::remove_file(target_dir.join(filename))?;
                    packet_infos.pop().unwrap();
                }
                File::create(target_dir.join("footer"))?
                    .write_all(&working_buffer[..discard_so_far + read_so_far])?;
                dump_packet_infos(&target_dir, &packet_infos)?;

                return Ok(());
            }
            Ok(n) => {
                let mut old_read_so_far = read_so_far;
                read_so_far = read_so_far + n;
                assert!(read_so_far <= PACKET_MAX_SIZE);
                assert!(discard_so_far <= DISCARD_MAX_SIZE);

                'try_again: loop {
                    for end_read_so_far in old_read_so_far..read_so_far {
                        if end_read_so_far < 3 {
                            continue;
                        }
                        running_crc.update(&[working_buffer[discard_so_far + end_read_so_far - 3]]);
                        let packet_info = PacketInfo::new(
                            packet_number,
                            packet_offset,
                            discard_so_far,
                            &working_buffer[discard_so_far..discard_so_far + end_read_so_far],
                            running_crc.get(),
                        );
                        if end_read_so_far < PACKET_MIN_SIZE {
                            continue;
                        }
                        if packet_info.crc == packet_info.target_crc && packet_info.crc != [0, 0] {
                            let filename = format!("packet{:05}", packet_number);
                            File::create(target_dir.join(filename))?.write_all(
                                &working_buffer[discard_so_far..discard_so_far + end_read_so_far],
                            )?;

                            if discard_so_far != 0 {
                                let filename = format!("packet{:05}-discard", packet_number);
                                File::create(target_dir.join(filename))?
                                    .write_all(&working_buffer[0..discard_so_far])?;
                            }

                            packet_infos.push(packet_info);
                            if read_so_far != end_read_so_far {
                                assert!(read_so_far > end_read_so_far);
                                assert!(read_so_far <= PACKET_MAX_SIZE);
                                assert!(discard_so_far <= DISCARD_MAX_SIZE);
                                unsafe {
                                    std::ptr::copy(
                                        &working_buffer[discard_so_far + end_read_so_far]
                                            as *const u8,
                                        &mut working_buffer[0] as *mut u8,
                                        read_so_far - end_read_so_far,
                                    );
                                }
                            }
                            packet_offset = packet_offset + end_read_so_far;
                            discard_so_far = 0;
                            read_so_far = read_so_far - end_read_so_far;
                            running_crc = crc16::State::<crc16::MODBUS>::new();
                            packet_number = packet_number + 1;

                            continue 'try_again;
                        }
                    }

                    if read_so_far == PACKET_MAX_SIZE && discard_so_far == DISCARD_MAX_SIZE {
                        dump_packet_infos(&target_dir, &packet_infos)?;
                        Err(Error::new(
                            ErrorKind::InvalidData,
                            "No packet found. Refuse to continue.",
                        ))?
                    } else if read_so_far == PACKET_MAX_SIZE {
                        packet_offset = packet_offset + 1;
                        discard_so_far = discard_so_far + 1;
                        running_crc = crc16::State::<crc16::MODBUS>::new();
                        read_so_far = read_so_far - 1;
                        old_read_so_far = 0;
                        continue 'try_again;
                    }

                    break;
                }
            }
            Err(ref err) if err.kind() == ErrorKind::Interrupted => (),
            err => return err.map(|_| unreachable!()),
        }
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
