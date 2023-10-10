/*
 * author: Adam Budziak
 */

#include <grafpe/domain/CSDC.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Basic test of CSDC") {
    using namespace grafpe;
    using namespace grafpe::domain;

    using prng_t = aes::openssl::AesCtr128PRNG;

    const auto key = hex_to_array<prng_t::key_type>("CAA63F5AC6CF2336E372FC9FDF716985");
    const auto  iv = hex_to_array<prng_t::iv_type>("B2C5BAA3E1E6E5B30583065DEBC9CEB0");

    auto range_100_crypter = crypter::ScalarGrafpe {
        key, iv,
        100,
        4,
        100
    };

    auto range_200_crypter = crypter::ScalarGrafpe {
        key, iv,
        200,
        4, 100
    };

    auto csdc = extension::CSDC<decltype(range_200_crypter), prng_t> {
        [&range_100_crypter](const point_t& v, const tweak_t& t) { return range_100_crypter.encrypt(v, t); },
        [&range_100_crypter](const point_t& v, const tweak_t& t) { return range_100_crypter.decrypt(v, t); },
        [](point_t p) { return p < 100; },
        [](point_t p) { return p < 100; },
        {key, iv}, 100
    };

    for (int i = 0; i < 100; ++i) {
        REQUIRE(csdc.encrypt(range_200_crypter, i) == range_100_crypter.encrypt(i));
    }

    std::vector<point_t> extended_domain(100);
    std::iota(extended_domain.begin(), extended_domain.end(), 100);
    for(auto& n: extended_domain) {
        n = csdc.encrypt(range_200_crypter, n);
    }

    using pur::stream_operators::operator<<;

    std::sort(extended_domain.begin(), extended_domain.end());
    for(std::size_t i = 0; i < extended_domain.size(); ++i) {
        REQUIRE(extended_domain[i] == i + 100);
    }
}
