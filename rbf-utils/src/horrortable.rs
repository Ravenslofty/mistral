#[macro_use]
mod utils;

#[macro_use]
extern crate static_assertions;

use itertools::Itertools;
use rayon::prelude::*;
use std::io::{self, Error, ErrorKind};
use std::path::{Path, PathBuf};

fn main() -> io::Result<()> {
    let mut args = std::env::args_os().skip(1);

    let mut files: Vec<_> = args.collect();
    let num_files = files.len();
    files.sort_unstable();
    files.dedup();
    if files.len() != num_files {
        eprintln!("Duplicate files found, ignoring.");
    }
    let num_files = files.len();

    let files: Vec<_> = files
        .into_iter()
        .map(|file| file.into())
        .map(|filepath: PathBuf| filepath.canonicalize())
        .collect::<Result<_, _>>()?;

    const VALID_BITPATERNS: [&'static str; 8] = [
        "5555555555555555",
        "AAAAAAAAAAAAAAAA",
        "CCCCCCCCCCCCCCCC",
        "F0F0F0F0F0F0F0F0",
        "FF00FF00FF00FF00",
        "FFFF0000FFFF0000",
        "FFFFFFFF00000000",
        "FFFFFFFFFFFFFFFF",
    ];
    let file_sets = files
        .into_iter()
        .map(|filepath| {
            (
                filepath
                    .file_name()
                    .unwrap()
                    .to_str()
                    .unwrap()
                    .split(|c| c == '-' || c == '.')
                    .map(|s| s.to_string())
                    .collect(),
                filepath,
            )
        })
        .filter(|(portions, _): &(Vec<_>, _)| portions.len() == 3)
        .filter(|(portions, _)| {
            VALID_BITPATERNS.contains(&(&portions[1] as &str))
        })
        .map(|(mut portions, filepath)| {
            let _ = portions.pop().unwrap();
            let input = portions.pop().unwrap();
            let id = portions.pop().unwrap();
            (id, input, filepath)
        })
        .group_by(|(id, _, _)| id.clone());

    let file_sets: Vec<_> = file_sets
        .into_iter()
        .map(|(id, files)| {
            let files: Vec<_> = files.collect();
            (id, files)
        })
        .collect();

    let file_sets_file_sum = file_sets
        .iter()
        .fold(0, |acc, (_, files)| acc + files.len());
    if file_sets_file_sum != num_files {
        eprintln!("Invalid file names found, ignoring.");
    }
    let num_file_sets = file_sets.len();

    let file_sets: Vec<_> = file_sets
        .into_par_iter()
        .map(|(id, mut files)| {
            files.sort_unstable_by(|(_, i1, _), (_, i2, _)| i1.cmp(&i2));
            (id, files.into_iter().map(|(_, i, p)| (i, p)).collect())
        })
        .filter(|(_, files): &(_, Vec<_>)| {
            files.len() == VALID_BITPATERNS.len()
        })
        .map(|(id, files)| {
            files
                .into_par_iter()
                .map(|filepath| {
                    utils::load_rbf(&filepath.1).map(|rbf| (rbf, filepath.0))
                })
                .collect::<Result<_, _>>()
                .map(|files: Vec<_>| (id, files))
        })
        .collect::<Result<_, _>>()?;

    if file_sets.len() != num_file_sets {
        eprintln!("Incomplete file sets found, ignoring.");
    }

    let target_len = file_sets[0].1.last().unwrap().0.len();
    let _: Vec<_> = file_sets
        .par_iter()
        .map(|(_, files)| {
            if files
                .par_iter()
                .map(|(rbf, _)| rbf.len() == target_len)
                .reduce(|| true, |acc, clen| if acc { clen } else { false })
            {
                Ok(())
            } else {
                make_io_error!("File lengths are not equal.")
            }
        })
        .collect::<Result<_, _>>()?;

    const_assert_eq!(utils::PACKET_MIN_SIZE, utils::PACKET_MAX_SIZE);
    let valid_range = {
        let sections = {
            let f = &file_sets[0].1.last().unwrap();
            utils::partition_rbf(&f.1, &f.0[..])
        };
        let first_packet = sections.iter().find(|s| match s.stype {
            utils::SectionType::Packet { .. } => true,
            _ => false,
        }).unwrap();
        let last_packet = sections.iter().rev().find(|s| match s.stype {
            utils::SectionType::Packet { .. } => true,
            _ => false,
        }).unwrap();

        first_packet.offset..last_packet.offset + last_packet.size
    };

    let file_sets: Vec<_> = file_sets
        //.into_par_iter()
        .into_iter()
        .map(|(id, mut files)| {
            dbg!(&files.iter().map(|a| a.1.clone()).collect::<Vec<_>>());
            let master = files.pop().unwrap();
            let diff_bytes = utils::find_diff_bytes(
                &mut files,
                master,
                1,
            );
            let diff_bits: Vec<_> = diff_bytes
                .into_iter()
                .filter(|(addr, _, _)| valid_range.contains(addr) && (addr - valid_range.start) % utils::PACKET_MAX_SIZE < utils::PACKET_MAX_SIZE - 8)
                .flat_map(|(addr, _, vals)| {
                    let id = id.clone();
                    (0..8u8).into_iter().filter_map(move |bit| {
                        let mut val = 0;
                        for b in 0..VALID_BITPATERNS.len() {
                            val |= (vals[b] >> bit & 0b1u8) << b;
                        }

                        match val {
                            0b1111_1111 => None,
                            0b0000_0000 => None,
                            _ => {
                                println!("{:0>8b} {:0>8b} {:x} {} {:x?} {:?}", val, !val, addr, bit, vals, id.clone());
                                Some((addr, bit, !val))
                            },
                        }
                    })
                })
                .collect();
            //dbg!(&diff_bits);
            (id, files, diff_bits)
        })
        .collect();

    Ok(())
}
