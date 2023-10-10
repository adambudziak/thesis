/*
 * author: Adam Budziak
 */

#include <algorithm>
#include <vector>

#include <grafpe/common/print_utils.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/domain/CycleSlicer.hpp>
#include <pur/logger/stream_operators.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"

TEST_CASE("CycleSlicer basic test") {
    using namespace grafpe;
    using Crypter = crypter::ScalarGrafpe<aes::openssl::AesCtr128PRNG>;

    using point_t = typename perm_t::value_type;

    const auto key = array_from_string<Crypter::key_type>("0000111122223333");
    const auto  iv = array_from_string<Crypter::iv_type>("3333222211110000");

    const point_t N = 30;
    const uint64_t D = 4;
    const uint64_t WALK_LENGTH = 4;

    auto alice = Crypter{key, iv, N, D, WALK_LENGTH};

    // inclusion function checks if both numbers are divisible by 5
    auto incl_func = [](uint32_t l, uint32_t r) {
        return !(l % 5) && !(r % 5);
    };

    domain::targeting::CycleSlicer<Crypter> slicer{
        {key, iv}, incl_func};

    vec_u32_t permutation(N);
    std::iota(permutation.begin(), permutation.end(), 0);

    vec_u32_t results(N);

    std::transform(permutation.begin(), permutation.end(), results.begin(),
            [&slicer, &alice](uint32_t x) { return slicer.encrypt(alice, x, 1000); });

    REQUIRE(results != permutation);

    SECTION("Items are changed iff match condition") {
        for (size_t i = 0; i < results.size(); ++i) {
            if (i % 5) {
                REQUIRE(i == results[i]);
            } else {
                REQUIRE(i == slicer.decrypt(alice, results[i], 1000));
            }
        }
    }
}
