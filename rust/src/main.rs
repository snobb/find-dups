//
// main.rs
// Author: Alex Kozadaev (2015)
//

extern crate crypto;

use std::env;
use std::fs;
use std::io::{self, Read};
use std::path::Path;

use crypto::md5;
use crypto::digest::Digest;

use std::collections::HashMap;

const VERSION : &'static str = "0.01";
const BLOCK_SIZE : usize = 128;

struct DupsData {
    data: HashMap<String, Vec<String>>,
    cnt: i32,
}

impl DupsData {
    fn new() -> DupsData {
        DupsData {
            data: HashMap::new(),
            cnt: 0
        }
    }

    // get an md5sum for the given file
    fn get_md5sum(&self, fname: &str) -> String {
        let fpath = Path::new(fname);
        let mut file = match fs::File::open(fpath) {
            Ok(f)   => io::BufReader::new(f),
            Err(e)  => panic!("couldn't open {}: {}", fname, e),
        };

        let mut md5 = md5::Md5::new();
        let mut buf = [0u8; BLOCK_SIZE];

        loop {
            match file.read(&mut buf) {
                Ok(0)       => break,
                Ok(_)       => md5.input(&mut buf),
                Err(e)      => panic!("{}: {}", fname, e),
            }
        }
        md5.result_str()
    }

    // record and display "progress bar"
    fn update_progress(&mut self) {
        self.cnt += 1;
        print!("\rprocessing: {}   ", self.cnt);
    }

    // process files and folders and fill the data database.
    fn process_folder(&mut self, dir: &Path) -> io::Result<()> {
        if try!(fs::metadata(dir)).is_dir() {
            for entry in try!(fs::read_dir(dir)) {
                let entry = try!(entry);
                if try!(fs::metadata(entry.path())).is_dir() {
                    try!(self.process_folder(&entry.path()));
                } else {
                    let file_path = entry.path();
                    let file = file_path.to_str().unwrap();
                    let key = self.get_md5sum(file);
                    if self.data.contains_key(&key) {
                        self.data.get_mut(&key).unwrap().push(file.to_string());
                    } else {
                        self.data.insert(key, vec![file.to_string()]);
                    }

                    self.update_progress();
                }
            }
        }
        Ok(())
    }

    // display the duplicates in the database.
    fn display_results(&mut self) {
        // output the result
        for (k, v) in self.data.iter() {
            if v.len() > 1 {
                println!("{}", k);
                for value in v.iter() {
                    println!("\t{}", value);
                }
            }
        }
    }
}

fn usage(prog: &str) {
    println!("{} v{}", prog, VERSION);
    println!("Usage: {} [directory/files to search]", prog);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let program = args[0].clone();

    // directories were not provided
    if args.len() == 1 {
        usage(&program);
    } else {
        // data storage (shared across the provided dirs)
        let mut dups_data = DupsData::new();

        // hangle the results
        for dir in args[1..].iter() {
            match dups_data.process_folder(&Path::new(dir)) {
                Ok(_)  => {},
                Err(_) => {},   // ignoring errors
            }
        }

        // display the duplicates
        dups_data.display_results();
    }
}

// vim: ts=4 sts=8 sw=4 tw=80 ai smarttab et fo=rtcq list
