use std::{
    io,
    path::{Path, PathBuf},
    process::Command,
};

use tempfile;

pub struct Quartus {
    path: PathBuf,
    verilog_files: Vec<PathBuf>,
}

impl Quartus {
    pub fn new<T: AsRef<Path>>(path: &T) -> Option<Self> {
        // force_os is for using Quartus Windows under WSL.
        #[cfg(windows)]
        let os = force_os.unwrap_or(OsType::Windows);
        #[cfg(unix)]
        let path: &Path = path.as_ref();

        if !path.join("quartus_sh.exe").exists() {
            println!("Couldn't find {}", path.join("quartus_sh.exe").display());
            return None;
        }

        Some(Quartus {
            path: path.to_owned(),
            verilog_files: Vec::new(),
        })
    }

    pub fn add_verilog(mut self, paths: Vec<PathBuf>) -> Self {
        for path in paths {
            self.verilog_files.push(path);
        }
        self
    }

    pub fn run_tcl(&self, cmd: &str, script: &str) -> io::Result<Vec<String>> {
        use io::Write;

        let mut tempfile = tempfile::NamedTempFile::new()?;

        tempfile.as_file_mut().write_all(script.as_bytes())?;

        let (_, path) = tempfile.into_parts();

        let cmd = Command::new(self.path.join(cmd))
            .arg("-t")
            .arg(&path)
            .output()?;

        let lines = String::from_utf8(cmd.stdout)
            .unwrap()
            .lines()
            .filter(|line| !line.contains("Info") && !line.contains("Error"))
            .map(String::from)
            .collect::<Vec<String>>();

        Ok(lines)
    }

    pub fn run_arg(&self, cmd: &str, args: &[&str]) -> io::Result<Vec<String>> {
        let cmd = Command::new(self.path.join(cmd)).args(args).output()?;

        let lines = String::from_utf8(cmd.stdout)
            .unwrap()
            .lines()
            .filter(|line| !line.contains("Info") && !line.contains("Error"))
            .map(String::from)
            .collect::<Vec<String>>();

        Ok(lines)
    }
}

mod tests {
    #[test]
    pub fn get_part_list() -> Result<(), Box<::std::error::Error>> {
        use crate::{OsType, Quartus};
        use std::fmt::Write;
        let q = Quartus::new(&"/mnt/d/intelFPGA_lite/18.1/quartus/bin64/").unwrap();
        let lines = q.run_tcl("quartus_cdb.exe", "puts [get_part_list]")?;

        assert!(lines.len() > 0);

        for line in lines {
            let mut cmd = String::new();
            for part in line.split_whitespace() {
                writeln!(
                    cmd,
                    "puts [get_part_info -family -device -package -pin_count {} ]",
                    part
                )?;
            }
            let data = q.run_tcl("quartus_cdb.exe", &cmd)?;
            for (metadata, part) in data.iter().zip(line.split_whitespace()) {
                println!("{}: {}", part, metadata);
            }
        }

        Ok(())
    }
}
