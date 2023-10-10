use criterion::*;

extern crate rand;
use rand::Rng;

extern crate openssl;
use openssl::symm::Cipher;



extern crate grafpe;
use grafpe::{
    graph::{ValidGraph, ConstructionMethod, srw::UndirectedSrw},
    irs::Irs,
    prng::Prng,
    crypt::Grafpe,
};

#[inline]
fn random_buffer(buffer: &mut [u8], rng: &mut rand::prelude::ThreadRng) {
    for i in 0..buffer.len() {
        buffer[i] = rng.gen::<u8>();
    }
}


fn run_bench(b: &mut Bencher, n: usize, d: usize, walk_length: usize, message_length: usize, constr: ConstructionMethod) {

    let mut rng = rand::thread_rng();
    let mut key = vec![0_u8; 16];
    let mut iv = vec![0_u8; 16];

    random_buffer(&mut key, &mut rng);
    random_buffer(&mut iv, &mut rng);

    let graph = ValidGraph::new(
        constr, n, d, &mut Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv)));
    random_buffer(&mut key, &mut rng);

    let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, walk_length);

    let mut msg = vec![0_u32; message_length];

    b.iter(|| {
        for i in 0..msg.len() {
            msg[i] = rng.gen::<u32>() % n as u32;
        }
        grafpe.encrypt(&msg, &[])
    })
}

fn criterion_benchmark(c: &mut Criterion) {
    let wl_100_4_128 = 101;
    c.bench_function(
        "ccn_100_4_128",
        move |b| { run_bench(b, 100, 4, wl_100_4_128, 8, ConstructionMethod::Naive) }
    );
    let wl_10000_4_128 = 148;
    c.bench_function(
        "ccn_10000_4_128",
        move |b| { run_bench(b, 10000, 4, wl_10000_4_128, 4, ConstructionMethod::Naive) }
    );
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
