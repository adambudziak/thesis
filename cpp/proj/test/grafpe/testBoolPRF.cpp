/*
 * author: Adam Budziak
 */

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/prf/RangeBoolPRF.hpp>
#include <grafpe/common/print_utils.hpp>
#include <pur/logger/Logger.hpp>
#include "../catch/catch.hpp"

TEST_CASE("RangeBoolPRF is just like frozen PRNG") {
    using namespace grafpe;
    using namespace grafpe::aes;

    using prng_t = openssl::AesCtr128PRNG;

    auto key_ = vec_from_string<vec_u8_t>("0000111122223333");
    auto  iv_ = vec_from_string<vec_u8_t>("3333222211110000");

    prng_t::key_type key;
    prng_t::iv_type iv;

    std::copy_n(key_.begin(), key.size(), key.begin());
    std::copy_n(iv_.begin(), iv.size(), iv.begin());

    const size_t RANGE_MAX = 37;

    auto prf = RangeBoolPRF{prng_t{key, iv}, RANGE_MAX};

    prng_t prng{key, iv};
    for (size_t i = 0; i < prf.size(); ++i) {
        REQUIRE(prng.fetch_bit() == prf(i));
    }

    auto another_prf = RangeBoolPRF{prng_t{key, iv}, RANGE_MAX};

    SECTION("PRFs based on equal PRNGs are equal") {
        REQUIRE(prf.size() == another_prf.size());
        for (size_t i = 0; i < prf.size(); ++i) {
            REQUIRE(prf(i) == another_prf(i));
        }
    }
}
