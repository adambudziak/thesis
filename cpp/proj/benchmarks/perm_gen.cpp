/*
 * author: Adam Budziak
 */

#include <iostream>

#include <grafpe/permutation/PermGen_Compat.hpp>
#include <grafpe/aes/openssl/AesCtr256PRNG.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>


int main(int argc, char **argv) {
    uint64_t SIZE;
    if (argc < 1) {
        std::cerr << "Set the size of permutation to generate";
        exit(1);
    }
    try {
        SIZE = std::stoull(argv[1]);
    } catch (const std::invalid_argument& /* exc */) {
        std::cerr << "The permutation size must be an unsigned integer";
        exit(1);
    }


    using prng_t = grafpe::aes::openssl::AesCtr256PRNG;
    namespace gp = grafpe::permutation;

    gp::Generator_Compat generator {prng_t{
        grafpe::random_array<prng_t::key_type>(grafpe::DummyPRNG{}),
        grafpe::random_array<prng_t::iv_type>(grafpe::DummyPRNG{})
    }};

    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    auto perm = generator.create_permutation(SIZE);
    auto stop = high_resolution_clock::now();

    std::cout << "Generated permutation of size " << perm.size() << '\n'
              << "Time elapsed: " << duration_cast<milliseconds>(stop - start).count()
              << std::endl;
}
