#![feature(seek_convenience)]

mod utils;

use rayon::prelude::*;
use std::fs::File;
use std::io::prelude::*;
use std::io::{Error, ErrorKind};
use std::path::Path;

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

                let rbf = utils::load_rbf(filename.as_ref())?;

                let sections =
                    utils::partition_rbf(filepath.as_os_str(), &rbf[..]);

                utils::dump_sections(&target_dir, &sections)?;

                let mut f = File::create(target_dir.join("sections"))?;
                for (i, section) in sections.iter().enumerate() {
                    write!(f, "SECTION START {:x},{:x}", section.size, i)?;

                    // Align to 16 bytes
                    const SPACES: [u8; 15] = [b' '; 15];
                    let align = (f.stream_position()? as usize + 1) % 16;
                    if align != 0 {
                        f.write_all(&SPACES[0..16 - align])?;
                    }
                    write!(f, "\n")?;

                    f.write_all(
                        &rbf[section.offset..section.offset + section.size],
                    )?;
                    write!(f, "\nSECTION END\n")?;
                }

                Ok(())
            })
            .unwrap_or_else(|err| {
                eprintln!("{:?}: Failed to process with {:?}", filename, err)
            });
    })
}
