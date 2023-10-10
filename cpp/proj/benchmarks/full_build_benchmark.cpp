/*
 * author: Adam Budziak
 */

#include <ostream>
#include <grafpe/common/print_utils.hpp>

#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/aes/openssl/AesCtr128.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/aes/openssl/AesCtr256PRNG.hpp>
#include <fstream>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>

constexpr const char * const BENCH_SEP = "*****************************************************";

/**
 * Stores the results of a benchmark.
 */

template<typename prng_t>
struct benchmark_ctx {
    using ctx_t = typename prng_t::ctx_type;
    uint64_t N;
    uint64_t D;
    uint64_t tests;
    typename prng_t::key_type key;
    typename prng_t::iv_type iv;
    constexpr static auto cipher_name = ctx_t::NAME;
};

template<typename prng_t>
int64_t run_benchmark(const benchmark_ctx<prng_t>& ctx) {
    using namespace grafpe;
    using namespace std::chrono;
    auto builder = GraphBuilder<prng_t>::create_from_engine({ctx.key, ctx.iv});
    builder.initialize();
    auto start = high_resolution_clock::now();
    builder.build(ctx.N, ctx.D);
    auto end = high_resolution_clock::now();

    return duration_cast<nanoseconds>(end - start).count();
}

template<typename prng_t>
void print_result_verbosely(std::ostream& os, const benchmark_ctx<prng_t>& ctx, double result) {
    os << BENCH_SEP << "\n\n\n"
       << "Benchmark for: N = " << ctx.N << ", D = " << ctx.D << '\n'
       << "Using: " << ctx.cipher_name << " in " << ctx.tests << " samples\n"
       << "Graph building time: " << result << "s"
       << "\n\n";
}

template<typename prng_t>
void print_result_data(std::ostream& os, const benchmark_ctx<prng_t>& ctx, double result) {
    os << ctx.N << "\t" << result << "\n";
}

void print_data_header(std::ostream& os) {
    os << "N\tD\ttime\n";
}

int main() {
    using namespace grafpe;
    using prng_t = aes::openssl::AesCtr128PRNG;

    benchmark_ctx<prng_t> bench {
        1000000,
        4,
        10
    };

    uint64_t start_N = 100, stop_N = 10000000, factor_N = 10;
    uint64_t start_D = 2, stop_D = 2, factor_D = 2;

    const auto output_filename = "/home/abudziak/repos/thesis/O3_AES-CTR-128.data";

    std::ofstream ofs;
    ofs.open(output_filename);
    print_data_header(ofs);

    for (auto D = start_D; D <= stop_D; D *= factor_D) {
        for (auto N = start_N; N <= stop_N; N *= factor_N) {
            bench.N = N;
            bench.D = D;
            double res = 0;
            std::cerr << '\n';
            for (auto i = 0; i < bench.tests; ++i) {
                std::cerr << '.';
                bench.key = random_array<prng_t::key_type>(DummyPRNG{});
                bench.iv  = random_array<prng_t::iv_type>(DummyPRNG{});
                res += run_benchmark(bench) / (double)(bench.tests);
            }
            std::cerr << '\n';
            res /= 1'000'000'000;
            print_result_verbosely(std::cout, bench, res);
//            print_result_data(std::cout, bench, res);
        }
    }
}
