/*
 * author: Adam Budziak
 */

#include <grafpe/common/print_utils.hpp>
#include "../catch/catch.hpp"

using namespace grafpe;

TEST_CASE("Conversion from bytes to hex") {
    SECTION("Simple array of unsigned char") {
        unsigned char bytes[] { 0xff, 0xee, 0xdd, 0x00, 0x11 };

        auto hexstr = bytes_to_hex(bytes, sizeof(bytes));

        REQUIRE(hexstr == "FFEEDD0011");
    }

    SECTION("As well as a vector of unsigned char") {
        vec_u8_t bytes{0xff, 0xee, 0xdd, 0x00, 0x11};

        REQUIRE(bytes_to_hex(bytes) == "FFEEDD0011");
    }
}

TEST_CASE("Conversion from hex to bytes") {
    vec_u8_t bytes {0xff, 0xee, 0xdd, 0x00, 0x11};
    REQUIRE(hex_to_bytes("FFEEDD0011") == bytes);
}

TEST_CASE("Hex->bytes is case-insensitive") {
    vec_u8_t bytes {0xff, 0xee, 0xdd, 0x00, 0x11};
    REQUIRE(hex_to_bytes("ffeedd0011") == bytes);
}

TEST_CASE("Odd-length hex are 0-padded from left") {
    vec_u8_t bytes {0x0f, 0xee, 0xdd, 0x00, 0x11};
    REQUIRE(hex_to_bytes("feedd0011") == bytes);
}
//
// TEST_CASE("Validation of hex-strings") {
//    std::string     hexstr{"FFEEDD012498037ABCDE"};
//    std::string not_hexstr{"EWLQUIHSDAUIFUDSALAS"};
//    std::string not_hexstr2{"FFEEDD012498037ABCDG"};
//
//    REQUIRE(is_hex(hexstr));
//    REQUIRE(!is_hex(not_hexstr));
//    REQUIRE(!is_hex(not_hexstr2));
//}
