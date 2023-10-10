extern crate criterion;
use criterion::*;

extern crate rand;
use rand::Rng;

extern crate openssl;
use openssl::symm::Cipher;

extern crate grafpe;
use grafpe::graph::{NewGraph };
use grafpe::irs::Irs;
use grafpe::prng::Prng;


fn run_bench(b: &mut Bencher, n: usize, d: usize) {

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
        NewGraph::new(n, d, &mut irs)
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
         "graph_4_aes_128_ctr_naive_new",
         ParameterizedBenchmark::new(
             "build graph time",
             |b, &&n| run_bench(b, n, 4),
             &[100_000, 1_000_000]
         ).sample_size(10).plot_config(plot_config.clone())
     );
     c.bench(
         "graph_8_aes_128_ctr_naive_new",
         ParameterizedBenchmark::new(
             "build graph time",
             |b, &&n| run_bench(b, n, 8),
             &[100_000, 1_000_000]
         ).sample_size(10).plot_config(plot_config.clone())
     );
     c.bench(
         "graph_32_aes_128_ctr_naive_new",
         ParameterizedBenchmark::new(
             "build graph time",
             |b, &&n| run_bench(b, n, 32),
             &[100_000, 1_000_000]
         ).sample_size(10).plot_config(plot_config.clone())
     );
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
