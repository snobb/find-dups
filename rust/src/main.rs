//
// main.rs
// Author: Alex Kozadaev (2015)
//

#[macro_use(print_err)]
extern crate finddups;
extern crate crypto;

use std::env;
use std::fs;
use std::io::{self, Read};
use std::path::Path;

use crypto::digest::Digest;
use crypto::md5;

use std::collections::HashMap;

const VERSION: &str = env!("CARGO_PKG_VERSION");
const BLOCK_SIZE: usize = 128;

struct DupsData {
    data: HashMap<String, Vec<String>>,
    cnt: i32,
}

impl DupsData {
    fn new() -> Self {
        Self {
            data: HashMap::new(),
            cnt: 0,
        }
    }

    // get an md5sum for the given file
    fn get_md5sum(&self, fpath: &Path) -> String {
        let fname = fpath.to_str().unwrap();

        let mut file = match fs::File::open(fpath) {
            Ok(f) => io::BufReader::new(f),
            Err(e) => panic!("couldn't open {}: {}", fname, e),
        };

        let mut md5 = md5::Md5::new();
        let mut buf = [0u8; BLOCK_SIZE];

        loop {
            match file.read(&mut buf) {
                Ok(0) => break,
                Ok(_) => md5.input(&buf),
                Err(e) => panic!("{}: {}", fname, e),
            }
        }

        md5.result_str()
    }

    // record and display "progress bar"
    fn update_progress(&mut self) {
        self.cnt += 1;
        print_err!("\rprocessing: {}   ", self.cnt);
    }

    // process files and folders and fill the data database.
    fn process_folder(&mut self, dir: &Path) -> io::Result<()> {
        if dir.is_dir() {
            for entry in fs::read_dir(dir)? {
                let path = entry?.path();

                if path.is_dir() {
                    self.process_folder(&path)?;
                } else if !path.is_file() {
                    continue;
                } else {
                    let fname = path.to_str().unwrap();
                    let key = self.get_md5sum(&path);
                    self.data.entry(key).or_default().push(String::from(fname));

                    self.update_progress();
                }
            }
        }

        Ok(())
    }

    // display the duplicates in the database.
    fn display_results(&mut self) {
        let mut got_dups = false;

        // output the result
        for (k, v) in self.data.iter() {
            if v.len() > 1 {
                got_dups = true;
                println!("{}", k);

                for value in v.iter() {
                    println!("\t{}", value);
                }
            }
        }

        if !got_dups {
            println!()
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
            dups_data.process_folder(Path::new(dir)).unwrap()
        }

        // display the duplicates
        dups_data.display_results();
    }
}
