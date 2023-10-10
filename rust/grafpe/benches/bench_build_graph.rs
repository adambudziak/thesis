extern crate criterion;
use criterion::*;

extern crate rand;
use rand::Rng;

extern crate openssl;
use openssl::symm::Cipher;

extern crate grafpe;
use grafpe::graph::{ValidGraph, ConstructionMethod };
use grafpe::irs::Irs;
use grafpe::prng::Prng;


fn run_bench(b: &mut Bencher, n: usize, d: usize, constr: ConstructionMethod) {

    let mut rng = rand::thread_rng();
    let mut key = vec![0_u8; 16];
    let iv = vec![0_u8; 16];

    b.iter(|| {
        for i in 0..16 {
            key[i] = rng.gen::<u8>();
        }
        let mut irs = Irs::new(
            Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv)
        );
        ValidGraph::new(constr.clone(), n, d, &mut irs)
    })
}

fn base(b: &mut Bencher) {
    let mut rng = rand::thread_rng();
    let mut key = vec![0_u8; 16];
    let iv = vec![0_u8; 16];

    b.iter(|| {
        for i in 0..16 {
            key[i] = rng.gen::<u8>();
        }
        Irs::new(
            Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv)
        );
    })
}

fn criterion_benchmark(c: &mut Criterion) {
     c.bench_function("base", base);
     let plot_config = PlotConfiguration::default()
         .summary_scale(AxisScale::Logarithmic);

     c.bench(
         "graph_4_aes_128_ctr_naive",
         ParameterizedBenchmark::new(
             "build graph time",
             |b, &&n| run_bench(b, n, 4, ConstructionMethod::Naive),
             &[10, 100, 1_000, 10_000]
         ).plot_config(plot_config.clone())
     );

     c.bench(
         "graph_4_aes_128_ctr_tree",
         ParameterizedBenchmark::new(
             "build graph time",
             |b, &&n| run_bench(b, n, 4, ConstructionMethod::Tree),
             &[10, 100, 1_000, 10_000]
         ).plot_config(plot_config.clone())
     );

     c.bench(
         "graph_8_aes_128_ctr_compare",
         ParameterizedBenchmark::new(
             "build graph time (naive)",
             |b, &&n| run_bench(b, n, 8, ConstructionMethod::Naive),
             &[10, 100, 1_000, 10_000]
         )
         .with_function(
             "build graph time (tree)",
             |b, &&n| run_bench(b, n, 8, ConstructionMethod::Tree),
         ).plot_config(plot_config.clone())
     );

     c.bench(
         "graph_4_aes_128_ctr_1000000",
         Benchmark::new(
             "build graph (naive)",
             |b| { run_bench(b, 1_000_000, 4, ConstructionMethod::Naive) }
         ).with_function(
             "build graph (tree)",
             |b| { run_bench(b, 1_000_000, 4, ConstructionMethod::Tree) }
         ).sample_size(10)
     );


    c.bench(
        "graph_8_aes_128_ctr_1000000",
        Benchmark::new(
            "build graph (naive)",
            |b| { run_bench(b, 1_000_000, 8, ConstructionMethod::Naive) }
        ).with_function(
            "build graph (tree)",
            |b| { run_bench(b, 1_000_000, 4, ConstructionMethod::Tree) }
        ).sample_size(10)
    );
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
