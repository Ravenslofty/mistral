//! To my dismay, binwalk is not fast enough.
//!
//! This two-pass diff algo devised by me in my boredom should hopefully
//! correct this.
mod utils;

use itertools::Itertools;
use rayon::prelude::*;
use std::io::{self, Error, ErrorKind};
use std::path::PathBuf;

use prettytable::{format, Cell, Row, Table};

macro_rules! make_io_error {
    ($err:expr $(, $($arg:tt)*)?) => (Err(Error::new(ErrorKind::Other, format!($err, $($($arg)*)? ) )));
}

#[derive(Debug, Clone)]
enum MatchType {
    Matching(u8),
    Varying(Vec<u8>),
}

const GREEN_COLOR: &'static str = "\x1B[32m";
const CYAN_COLOR: &'static str = "\x1B[36m";
const BRIGHT_GREEN_COLOR: &'static str = "\x1B[32;1m";
const BRIGHT_CYAN_COLOR: &'static str = "\x1B[36;1m";
const BOLD_COLOR: &'static str = "\x1B[1m";
const RESET_COLOR: &'static str = "\x1B[0m";

fn main() -> io::Result<()> {
    let mut files: Vec<_> = std::env::args_os()
        .skip(1)
        .map(|file| file.into())
        .map(|filepath: PathBuf| filepath.canonicalize())
        .collect::<Result<_, _>>()?;
    let num_files = files.len();
    files.sort_unstable();
    files.dedup();
    if files.len() < 2 {
        make_io_error!("Need at least two files to diff.")?;
    }

    if files.len() != num_files {
        eprintln!("Duplicate files found, ignoring.");
    }

    eprintln!("Loading {} files...", files.len());

    let mut files: Vec<_> = files
        .into_iter()
        .map(|filepath| utils::load_rbf(&filepath).map(|rbf| (rbf, filepath)))
        .collect::<Result<_, _>>()?;

    let master_file = files.pop().unwrap();

    eprintln!("Validating sizes...");

    files
        .par_iter()
        .map(|(rbf, filepath)| {
            if rbf.len() == master_file.0.len() {
                Ok(())
            } else {
                make_io_error!("{:?}: length does not match.", filepath)
            }
        })
        .collect::<Result<_, _>>()?;

    let diff_bytes = utils::find_diff_bytes(&mut files, master_file);

    eprintln!("Proccesing lines...");

    let diff_bytes_groups =
        diff_bytes.into_iter().group_by(|(_, line, _)| *line);
    let diff_lines: Vec<_> = diff_bytes_groups
        .into_iter()
        .map(|(line, line_contents)| {
            let mut line_contents: Vec<_> =
                line_contents.map(|lc| Some(lc)).collect();
            let mut offset = 0;

            let start = line * utils::BYTES_PER_LINE;
            let end = start + utils::BYTES_PER_LINE;

            let mut match_types = vec![];

            'to_next_byte: for byte in start..end {
                while offset < line_contents.len()
                    && line_contents[offset].as_ref().unwrap().0 <= byte
                {
                    if line_contents[offset].as_ref().unwrap().0 == byte {
                        match_types.push(MatchType::Varying(
                            line_contents[offset].take().unwrap().2,
                        ));
                        offset += 1;
                        continue 'to_next_byte;
                    }
                    offset += 1;
                }
                match_types.push(MatchType::Matching(files[0].0[byte]));
            }
            (start, match_types)
        })
        .collect();

    let mut table = Table::new();
    let format = format::FormatBuilder::new()
        .column_separator('|')
        .borders('|')
        .padding(1, 1)
        .build();
    table.set_format(format);
    let mut header = vec![Cell::new("addr")];
    header.extend(
        files
            .iter()
            .map(|(_, filepath)| Cell::new(filepath.to_str().unwrap())),
    );
    table.add_row(Row::new(header));

    eprintln!("Generating info for {} lines...", diff_lines.len());

    let diff_rows: Vec<_> = diff_lines
        .into_par_iter()
        .map(|(start, match_types)| {
            let mut this_row = Vec::with_capacity(match_types.len() + 1);
            this_row.push(Cell::new(&format!("0x{:0>8x}", start)));

            this_row.extend(files.iter().enumerate().map(|(i, _)| {
                let mut main_str = String::new();
                let mut preview_str = String::new();

                preview_str.push_str("|");

                let mut first = true;
                for byte in 0..utils::BYTES_PER_LINE {
                    if !first {
                        main_str.push_str(" ");
                    }
                    first = false;

                    let (dim_color, color, val) = match match_types[byte] {
                        MatchType::Matching(v) => {
                            (GREEN_COLOR, BRIGHT_GREEN_COLOR, v)
                        }
                        MatchType::Varying(ref vs) => {
                            (CYAN_COLOR, BRIGHT_CYAN_COLOR, vs[i])
                        }
                    };

                    main_str.push_str(&format!("{}{:0>2x}", color, val,));

                    if val.is_ascii_graphic() || val == b' ' {
                        preview_str.push_str(&format!(
                            "{}{}{}{}",
                            BOLD_COLOR,
                            color,
                            char::from(val),
                            RESET_COLOR,
                        ));
                    } else {
                        preview_str.push_str(&format!("{}.", dim_color,));
                    };
                }

                preview_str.push_str(&format!("{}|", RESET_COLOR));
                let mut out_str = main_str;
                out_str.push_str(&format!("{} ", RESET_COLOR));
                out_str.push_str(&preview_str);

                Cell::new(&out_str)
            }));

            Row::new(this_row)
        })
        .collect();

    eprintln!("Adding {} rows...", diff_rows.len());

    diff_rows.into_iter().for_each(|row| {
        table.add_row(row);
    });
    table.printstd();

    Ok(())
}
