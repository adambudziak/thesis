use std::io::{Write};
use std::fs::File;

use std::error::Error;

use openssl::symm::{Cipher, Crypter, Mode};
use grafpe::graph::{ValidGraph, Graph};
use grafpe::{graph, irs, prng};
use grafpe::graph::convertible::{Convertible, GraphFormat};

use rand::Rng;

use matrust::*;

fn main() -> Result<(), Box<dyn Error>> {
    let mut rng = rand::thread_rng();
    let key: [u8; 16] = rng.gen::<[u8; 16]>();
//    let key = hex::decode("01230123012301230123012301230123").unwrap();
    let iv = hex::decode("00001111222233330000111122223333").unwrap();
    let prng = prng::Prng::new(
        Crypter::new(Cipher::aes_128_ctr(), Mode::Encrypt, &key, Some(&iv)).unwrap(),
    );

    let graph = ValidGraph::new(
        graph::ConstructionMethod::Naive,
        10000,
        4,
        &mut irs::Irs::new(prng),
    );

    let mut file = File::create("current.dot")?;
    file.write_all(graph.to_string_undirected(GraphFormat::Dot).as_bytes());

    let rotated = rotate_permutations(graph.permutations());
    let mut matrix = permutations_to_matrix(&rotated);

    for i in 0..1200 {
        let tvd = compute_tvd(&matrix);
        let log2 = tvd.clone().log2();
        println!("{}\t{}", i, tvd);
//        matrix = &matrix * &matrix;
        matrix = multiply_matrix(&rotated, &matrix);
    }

    Ok(())
}