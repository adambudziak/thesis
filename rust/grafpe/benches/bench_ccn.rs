use criterion::*;

use rand::Rng;

use grafpe::{
    graph::{ValidGraph, ConstructionMethod, srw::UndirectedSrw},
    irs::Irs,
    prng::Prng,
    crypt::Grafpe,
    fpe::ccn::{Digit16Ccn, luhn}
};
use openssl::symm::Cipher;
use rand::rngs::ThreadRng;

fn random_luhn(rng: &mut ThreadRng) -> u64 {
    let mut res = rng.gen_range(1, 10_u64.pow(16));
    while !luhn(res) {
        res = rng.gen_range(1, 10_u64.pow(16));
    }
    res
}

fn ccn_benchmark(c: &mut Criterion) {
    let mut rng = rand::thread_rng();
    let key = rng.gen::<[u8; 16]>();
    let iv = rng.gen::<[u8; 16]>();

    let mut irs = Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv));


    c.bench_function("ccn bench", move |b| {
        let graph = ValidGraph::new(ConstructionMethod::Naive, 10000, 4, &mut irs);
        let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, 64);
        let input = random_luhn(&mut rng);
        b.iter(|| {
            Digit16Ccn::new(input).encrypt_with_luhn(&grafpe);
        })
    });
}

criterion_group!(benches, ccn_benchmark);
criterion_main!(benches);
