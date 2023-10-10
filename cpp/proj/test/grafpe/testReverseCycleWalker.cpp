/*
 * author: Adam Budziak
 */

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/domain/ReverseCycleWalker.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"

TEST_CASE("ReverseCycleWalker basic test") {
    using namespace grafpe;
    using Crypter = crypter::ScalarGrafpe<aes::openssl::AesCtr128PRNG>;
    using domain::targeting::ReverseCycleWalker;

    using point_t = typename perm_t::value_type;

    const auto key = array_from_string<Crypter::key_type>("0000111122223333");
    const auto  iv = array_from_string<Crypter::iv_type>("0123012301230123");

    const point_t N = 256;
    const uint64_t D  = 4;
    const uint64_t WALK_LENGTH = 64;
    const uint64_t ROUNDS_NUMBER = 1000;

    auto alice = Crypter{key, iv, N, D, WALK_LENGTH};

    auto domain_fn = [](point_t x) {
        return !(x % 17);      // recognizes numbers divisible by 17
    };

    ReverseCycleWalker<Crypter> rcw{vec_u8_t{key.begin(), key.end()}, domain_fn};

    perm_t permutation(N);
    std::iota(permutation.begin(), permutation.end(), 0);

    std::transform(permutation.begin(), permutation.end(), permutation.begin(),
            [&rcw, &alice, &domain_fn, ROUNDS_NUMBER](point_t x) {
        return domain_fn(x) ? rcw.encrypt(alice, x, ROUNDS_NUMBER) : x;
    });

    SECTION("Points are are permuted iff they're in the subdomain") {
        for (point_t i = 0; i < permutation.size(); ++i) {
            if (domain_fn(i)) {
                REQUIRE(rcw.decrypt(alice, permutation[i], ROUNDS_NUMBER) == i);
            }
        }
    }
}
