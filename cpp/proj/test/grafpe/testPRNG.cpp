/*
 * author: Adam Budziak
 */

#include <grafpe/common/print_utils.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Test that fetched bits match the engine-generated sequence") {
    using namespace grafpe;
    using namespace grafpe::aes;

    using engine_t = openssl::AesCtr128PRNG;

    const auto key = array_from_string<engine_t::key_type>("0000111122223333");
    const auto  iv = array_from_string<engine_t::iv_type>("3333222211110000");

    engine_t prng_engine{key, iv};

    vec_u8_t buffer(32, '\0');
    prng_engine.fill_bytes(buffer);


    engine_t prng{key, iv};

    vec_u8_t result(32, '\0');
    for (size_t i = 0; i < result.size() * 8; ++i) {
        result[i / 8] |= (prng.fetch_bit() << i % 8);
    }
    REQUIRE(buffer == result);
}

TEST_CASE("Multi-threaded PRNG is equivalent to the single-threaded one") {
    using namespace grafpe;
    using namespace grafpe::aes;

    using prng_t    = openssl::AesCtr128PRNG;
    using prng_mt_t = openssl::AesCtr128PRNG_MT;

    const auto key = array_from_string<prng_t::key_type>("0000111122223333");
    const auto  iv = array_from_string<prng_t::iv_type>("3333222211110000");

    auto prng    = prng_t{key, iv};
    auto prng_mt = prng_mt_t{key, iv};

    prng.initialize();
    prng_mt.initialize();

    for (int i = 0; i < 10000; ++i) {
        REQUIRE(prng.fetch_bit() == prng_mt.fetch_bit());
    }
}
