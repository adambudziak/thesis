/*
 * author: Adam Budziak
 */

#include <grafpe/common/print_utils.hpp>
#include <grafpe/crypt/utils.hpp>
#include <iostream>
#include <bitset>
#include "../catch/catch.hpp"

TEST_CASE("Tests of get bit") {
    using namespace grafpe;

    auto buffer = hex_to_bytes("00102030");

    vec_u8_t result(buffer.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
        for (size_t bit = 0; bit < 8; ++bit)
            result[i] |= get_bit(buffer, i * 8 + bit) << (7 -bit);
    }
    REQUIRE(result == buffer);
}

TEST_CASE("Tests of get_bits from a number") {
    using namespace grafpe;

    uint32_t num = 0xF9abcdef;
    REQUIRE(get_bits(num, 0, 20) == 0xf9abc);
    REQUIRE(get_bits(num, 5, 20) == 0x1abc);
    REQUIRE(get_bits(num, 5, 15) == 0xd5);
    REQUIRE(get_bits(num, 0, 32) == num);
    REQUIRE(get_bits(num, 0, 2) == 3);
    REQUIRE(get_bits(0x70000000, 0, 2) == 1);
}

TEST_CASE("Test get bits from a vector") {
    using namespace grafpe;


    WHEN("Working on a 32-bit buffer") {
        vec_u8_t buffer = hex_to_bytes("F2345678");
        uint32_t num = 0xf2345678;
        // buffer == 11110010001101000101011001111000
        REQUIRE(get_bits(buffer, 10, 32) == get_bits(num, 10, 32));
        REQUIRE(get_bits(buffer, 2, 15) == get_bits(num, 2, 15));
    }

    WHEN("Working on a longer buffer") {
        uint32_t num = 0xf2345678;
        vec_u8_t buffer = hex_to_bytes("0000000000000000000000F2345678");
        REQUIRE(get_bits(buffer, buffer.size() * 8 - 32, buffer.size() * 8) == get_bits(num, 0, 32));
        REQUIRE(get_bits(buffer, buffer.size() * 8 - 48, buffer.size() * 8 - 16) == get_bits(0xf234, 0, 32));
    }
}

TEST_CASE("Get bits from buffer") {
    using namespace grafpe;

    auto buffer = hex_to_bytes("77345678");
    // buffer == 01110111 00110100 01010110 01111000

    REQUIRE(get_bits(buffer, 0, 2) == 1);
    REQUIRE(get_bits(buffer, 1, 3) == 3);
    REQUIRE(get_bits(buffer, 5, 8) == 7);
    REQUIRE(get_bits(buffer, 6, 9) == 6);
    REQUIRE(get_bits(buffer, 20, 27) == 51);
}

TEST_CASE("Get permutation index") {
    using namespace grafpe;


    auto buffer = hex_to_bytes("12345678");
    // buffer == 00010010001101000101011001111000
    REQUIRE(get_permutation_index(buffer, 1, 0) == 0);
    REQUIRE(get_permutation_index(buffer, 1, 3) == 1);
    REQUIRE(get_permutation_index(buffer, 1, 12) == 0);
    REQUIRE(get_permutation_index(buffer, 1, 14) == 1);

    REQUIRE(get_permutation_index(buffer, 2, 1) == 2);
    REQUIRE(get_permutation_index(buffer, 2, 2) == 2);

    REQUIRE(get_permutation_index(buffer, 4, 0) == 1);

    REQUIRE(get_permutation_index(buffer, 8, 0) == 18);
}
