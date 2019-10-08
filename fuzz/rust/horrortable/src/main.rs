#![forbid(unsafe_code)]

use std::{
    error::Error,
    env,
    fs,
    io,
};

fn main() -> Result<(), Box<dyn Error>> {
    let mut files = Vec::new();

    assert!(env::args().count() < 64);

    // Load into memory
    let mut bitstream_length: Option<usize> = None;
    for file in env::args().skip(1) {
        files.push(fs::read(file)?);
        let last_length = files[files.len()-1].len();
        match bitstream_length {
            Some(bitstream_length) => assert_eq!(bitstream_length, last_length),
            None => bitstream_length = Some(last_length),
        }
    }

    let bitstream_length = bitstream_length.unwrap();
    let mut data = vec![0_u64; bitstream_length * 8];

    // Transpose 
    for (file_number, file) in files.iter().enumerate() {
        for (byte_number, byte) in file.iter().enumerate() {
            let bit_number = byte_number * 8;
            for i in 0..=7 {
                let bit = (byte >> i) & 1;
                data[bit_number + i] |= u64::from(bit << file_number);
            }
        }
    }

    for pattern in 1..=126 {
        let pattern = pattern as u64;
        let mut iter = data.iter().enumerate();
        while let Some((index, pattern)) = iter.find(|(_, x)| **x == pattern as u64) {
            println!("Found pattern {:08b} at bit {} (byte {:05x}, bit {})", pattern, index, index / 8, index % 8);
        }
    }

    Ok(())
}
