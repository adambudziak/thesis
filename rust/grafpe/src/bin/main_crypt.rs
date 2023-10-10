#[macro_use]
extern crate clap;
use clap::{App, ArgMatches};

extern crate hex;
use hex::decode;

use std::{error::Error, fmt};

use std::io::{self, BufRead};

extern crate grafpe;
use grafpe::graph::ValidGraph;
use grafpe::graph::{Walker, to_perm, srw::UndirectedSrw};
use grafpe::crypt::Grafpe;
use grafpe::graph::convertible::dot;

#[derive(Debug)]
pub struct ParseKeyError {
    actual_len: usize,
    expected_len: usize,
}

impl Error for ParseKeyError {}


impl fmt::Display for ParseKeyError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Key parsing error! Expected length: {} chars, actual: {}",
               self.expected_len, self.actual_len)
    }
}


fn assert_key_valid(key: &str) -> Result<(), Box<Error>> {
    const CHARS_PER_BYTE: usize = 2;
    const BYTES_PER_KEY: usize = 16;
    const EXPECTED_LEN: usize = CHARS_PER_BYTE * BYTES_PER_KEY;
    if key.len() != EXPECTED_LEN {
        return Err(Box::new(ParseKeyError{
            actual_len: key.len(),
            expected_len: EXPECTED_LEN
        }));
    }
    decode(key)?;

    Ok(())
}


enum GraphFileType {
    Dot,
    Perm
}

enum Mode {
    Encrypt,
    Decrypt
}


fn load_graph(filepath: &str, filetype: GraphFileType) -> Result<ValidGraph, Box<Error>> {
    match filetype {
        GraphFileType::Perm => to_perm::from_perm_directed(filepath),
        GraphFileType::Dot => dot::perms_from_undirected(filepath),
    }
}

fn read_msg_from_stdin() -> Result<Vec<u32>, Box<Error>> {
    let stdin = io::stdin();
    let mut line = String::new();
    stdin.lock().read_line(&mut line)?;

    line.pop();
    let split = line.split(" ");
    let (lower_bound, _) = split.size_hint();

    let mut msg: Vec<u32> = Vec::with_capacity(lower_bound);
    for s in split {
        let point = s.parse::<u32>()?;
        msg.push(point);
    }
    Ok(msg)
}

fn run<W: Walker>(grafpe: Grafpe<W>, msg: Vec<u32>, mode: Mode) {
    let result = match mode {
        Mode::Encrypt => grafpe.encrypt(&msg, &[]),
        Mode::Decrypt => grafpe.decrypt(&msg, &[]),
    };

    println!("result: {:?}", result);
}


fn get_walk_length(matches: &ArgMatches) -> Result<usize, Box<Error>> {
    let wl: usize = if matches.is_present("wl-from-perm") {
        let filepath = matches.value_of("wl-from-perm").unwrap();
        let wl_ = to_perm::walk_length_from_perm(filepath)?;
        wl_
    } else {
        matches.value_of("walk-length").unwrap().parse::<usize>()?
    };

    Ok(wl)
}


fn main() -> Result<(), Box<Error>> {
    let yaml = load_yaml!("./cli.yml");
    let matches = App::from_yaml(yaml).get_matches();
    let key = matches.value_of("key").unwrap();
    assert_key_valid(&key)?;
    let key = hex::decode(&key).unwrap();

    let mut graph_filetype: GraphFileType = GraphFileType::Dot;
    let filepath: &str;
    if let Some(filepath_) = matches.value_of("perm-file") {
        filepath = filepath_;
        graph_filetype = GraphFileType::Perm;
    } else {
        filepath = matches.value_of("dot-file").unwrap();
    }
    let graph = load_graph(&filepath, graph_filetype)?;

    let walk_length = get_walk_length(&matches)?;

    // println!("Graph: {}", to_dot::to_dot_directed(&graph));
    let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, walk_length);

    let msg = read_msg_from_stdin()?;

    let mode = if matches.is_present("decrypt") { Mode::Decrypt }
               else { Mode::Encrypt };

    run(grafpe, msg, mode);

    Ok(())
}
