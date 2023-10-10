/*
 * author: Adam Budziak
 */

#include <grafpe/common/print_utils.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/domain/CycleWalker.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"


TEST_CASE("CycleWalker basic test") {
    using namespace grafpe;
    using Crypter = crypter::ScalarGrafpe<aes::openssl::AesCtr128PRNG>;
    using CycleWalker = grafpe::domain::targeting::CycleWalker<Crypter>;

    auto key = array_from_string<Crypter::key_type>("0000111122223333");
    auto  iv = array_from_string<Crypter::iv_type>("3333222211110000");

    const point_t N = 1000;
    const uint64_t D = 4;
    const uint64_t WALK_LENGTH = 4;

    auto alice = Crypter{key, iv, N, D, WALK_LENGTH};

    CycleWalker walker{[](const point_t& num) { return !(num % 5); }};

    using namespace std::placeholders;
    using cw_fn_t = point_t(CycleWalker::*)(const Crypter&, const point_t&) const;

    auto dec = std::bind(static_cast<cw_fn_t>(&CycleWalker::decrypt), walker, alice, _1);
    auto enc = std::bind(static_cast<cw_fn_t>(&CycleWalker::encrypt), walker, alice, _1);

    SECTION("dec(enc(M)) == M") {
        uint32_t inputs[] = {
                5, 10, 20, 55, 400, 495, 500, 800, 995
        };
        for (auto& input : inputs) {
            REQUIRE(dec(enc(input)) == input);
        }
    }

    SECTION("Encrypted values create a permutation") {
        perm_t ciphers(N / 5);
        for (point_t num = 0; num < N; num += 5) {
            ciphers[num / 5] = enc(num) / 5;
        }
        REQUIRE(is_permutation(ciphers));
    }
}
