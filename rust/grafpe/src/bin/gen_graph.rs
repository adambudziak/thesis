#[macro_use]
extern crate clap;
use clap::{App, ArgMatches};

extern crate hex;
use hex::decode;

use std::{
    error::Error,
    fmt,
    fs::File,
    io::Write,
};

extern crate grafpe;
use grafpe::{
    graph::{
        ValidGraph,
        ConstructionMethod,
    },
    irs::Irs,
    prng::Prng,
};

use openssl::symm::Cipher;
use grafpe::graph::NewGraph;
use grafpe::graph::Graph;
use grafpe::graph::convertible::{Convertible, GraphFormat};

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

fn get_vertex_count(matches: &ArgMatches) -> Result<usize, Box<Error>> {
    let vertex_count = matches.value_of("n").unwrap();
    let vertex_count = vertex_count.parse::<usize>()?;
    Ok(vertex_count)
}

fn get_degree(matches: &ArgMatches) -> Result<usize, Box<Error>> {
    let degree = matches.value_of("d").unwrap();
    let degree = degree.parse::<usize>()?;
    Ok(degree)
}

fn get_walk_length(matches: &ArgMatches, n: usize, d: usize)
    -> Result<usize, Box<Error>>
{
    if matches.is_present("walk-length")  {
        let wl = matches.value_of("walk-length").unwrap().parse::<usize>()?;
        Ok(wl)
    } else {
        let d = d as f32;
        let n = n as f32;
        Ok((d / (d - 2.) * n.log(d - 1.) * 2.) as usize)
    }
}

enum GraphFileType {
    Dot,
    Perm
}

fn get_graph_file_meta(matches: &ArgMatches) -> (String, GraphFileType) {
    let graph_filetype = GraphFileType::Dot;
    let filepath = matches.value_of("export-dot").unwrap();
    (String::from(filepath), graph_filetype)
}

fn export_graph<G: Graph + Convertible>(filepath: &str,
                          filetype: GraphFileType,
                          graph: &G)
                          -> std::io::Result<()>
{
    match filetype {
        GraphFileType::Perm => {
//            let graph_string = to_perm::to_perm_directed(&graph, walk_length);
//            let mut file = File::create(filepath)?;
//            file.write_all(graph_string.as_bytes())?;
//            Ok(())
            unimplemented!()
        },
        GraphFileType::Dot => {
            let graph_string = graph.to_string_undirected(GraphFormat::Dot);
            let mut file = File::create(filepath)?;
            file.write_all(graph_string.as_bytes())?;
            Ok(())
        }
    }
}

fn main() -> Result<(), Box<Error>> {
    let yaml = load_yaml!("gen_graph_cli.yml");
    let matches = App::from_yaml(yaml).get_matches();
    let key = matches.value_of("key").unwrap();
    let  iv = matches.value_of("iv").unwrap();

    assert_key_valid(&key)?;
    assert_key_valid(&iv)?;

    let key = hex::decode(&key).unwrap();
    let  iv = hex::decode(&iv).unwrap();

    let n = get_vertex_count(&matches)?;
    let d = get_degree(&matches)?;

    let _walk_length = get_walk_length(&matches, n, d)?;

    let (filepath, filetype) = get_graph_file_meta(&matches);

    let wants_new_graph = matches.is_present("new-graph");
    println!("{}", wants_new_graph);

    let graph: Box<dyn Graph> = if wants_new_graph {
        Box::new(
            NewGraph::new(
            n, d,
            &mut Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv))
        ))
    } else {
        Box::new(ValidGraph::new(
            ConstructionMethod::Naive,
            n, d,
            &mut Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv))
        ))
    };

    match graph.as_any().downcast_ref::<ValidGraph>() {
        Some(g) => export_graph(&filepath, filetype, g)?,
        None => match graph.as_any().downcast_ref::<NewGraph>() {
            Some(g) => export_graph(&filepath, filetype, g)?,
            None => panic!("Invalid graph type!")
        }
    }

    Ok(())
}
