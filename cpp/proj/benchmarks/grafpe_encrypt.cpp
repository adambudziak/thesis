/*
 * author: Adam Budziak
 */

#include <chrono>

#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>


int main() {
    using namespace grafpe;
    using namespace grafpe::crypter;
    using prng_t = grafpe::aes::openssl::AesCtr128PRNG;

    using namespace std::chrono;

    int tests = 100;
    uint64_t N = 1000, D = 4;
    uint64_t walk_length = 100;

    perm_t vec(1, 0);

    double result = 0;

    for (int i = 0; i < tests; ++i) {
        std::cout << '.';
        auto grafpe = Grafpe {
            random_array<prng_t::key_type>(DummyPRNG{}),
            random_array<prng_t::iv_type>(DummyPRNG{}),
            N,
            D,
            walk_length
        };

        std::cout << '+';

        auto start = high_resolution_clock::now();
        vec = grafpe.encrypt(vec);
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<nanoseconds>(end - start).count();
        result += duration / (double)(tests);
    }

    std::cout << '\n' << N << '\t' << D << '\t' << walk_length << '\t' << result / 1'000'000'000 << '\n';
}