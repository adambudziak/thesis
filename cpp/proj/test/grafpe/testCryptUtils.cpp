/*
 * author: Adam Budziak
 */

#include <cstring>
#include "../catch/catch.hpp"

#include <grafpe/crypt/utils.hpp>
#include <grafpe/aes/openssl/AesCtr128.hpp>
#include <grafpe/common/print_utils.hpp>
#include <iostream>
#include <pur/logger/stream_operators.hpp>

TEST_CASE("Test of generating key and iv") {
    using namespace grafpe;

    // crypt engine creates keys and ivs deterministically
    WHEN("Using a short digest") {
        vec_u8_t digest(4, 0);

        using ctx_t = aes::openssl::AesCtr128::ctx_type;
        auto KEY = ctx_t::key_type {};
        auto  IV = ctx_t::iv_type {};

        generate_key_iv<ctx_t>(digest, KEY, IV);

        REQUIRE(bytes_to_hex(KEY) == "F1D3FF8443297732862DF21DC4E57262");
        REQUIRE(bytes_to_hex(IV) == "4F7BDB3CDE517A214DA5CFF2C447F86D");
    }

    WHEN("Using an empty digest") {
        vec_u8_t digest {0};

        using ctx_t = aes::openssl::AesCtr128::ctx_type;
        auto KEY = ctx_t::key_type {};
        auto  IV = ctx_t::iv_type {};

        generate_key_iv<ctx_t>(digest, KEY, IV);

        REQUIRE(bytes_to_hex(KEY) == "93B885ADFE0DA089CDF634904FD59F71");
        REQUIRE(bytes_to_hex(IV) == "CAB338C156D42203FF557885C1D6A2EA");
    }
}

TEST_CASE("Test of get digest") {
    using namespace grafpe;

    perm_t msg{1};
    perm_t abs{0};

    REQUIRE(get_digest(msg, abs, 0) == vec_u8_t{0});
}
