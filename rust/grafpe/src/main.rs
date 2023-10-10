use openssl::symm::{decrypt, encrypt, Cipher, Crypter, Mode};

//use std::fs::File;
//use std::io::Write;

use grafpe::graph::{self, ValidGraph, to_perm::from_perm_directed, convertible::Convertible, is_valid, srw::UndirectedSrw, SecureDirected, ConstructionMethod};
use grafpe::crypt::Grafpe;
use grafpe::irs;
use grafpe::prng;

use grafpe::graph::NewGraph;
use grafpe::graph::srw::DirectedSrw;
use grafpe::graph::convertible::GraphFormat;

fn main() {
    let key = hex::decode("01230123012301230123012301230123").unwrap();
    let iv = hex::decode("00001111222233330000111122223333").unwrap();
    let prng = prng::Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv);
//    let graph = NewGraph::new(100, 4, &mut irs::Irs::new(prng));
    let graph = ValidGraph::new(ConstructionMethod::Naive, 100, 4, &mut irs::Irs::new(prng));

    let walk_length = graph.directed_walk_length(128);

    println!("Graph: {}", graph.to_string_directed(GraphFormat::Dot));
    println!("Walk length: {}", walk_length);
    println!("Is valid? {}", is_valid(&graph));

    let crypter = Grafpe::new(DirectedSrw::new(&graph), &key, walk_length);
    let cipher = crypter.encrypt(&[1, 2, 3, 4, 5, 6, 7, 8, 9], &[]);
    println!("CIPHER: {:?}", cipher);
    let decr = crypter.decrypt(&cipher, &[]);
    println!("DECR: {:?}", decr);
}
