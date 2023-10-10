/*
 * author: Adam Budziak
 */

#include <dfare/common/Alphabet.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Test alphabet invariants") {
    using namespace dfare;

    Alphabet a{"bcdea"};

    REQUIRE(a.ord('a') == 0);
    REQUIRE(a.ord('b') == 1);
    REQUIRE(a.ord('c') == 2);
    REQUIRE(a.ord('d') == 3);
    REQUIRE(a.ord('e') == 4);

    a.add_character('g');
    REQUIRE(a.ord('g') == 5);

    a.add_character('i');
    REQUIRE(a.ord('i') == 6);

    a.add_character('f');
    REQUIRE(a.ord('f') == 5);
    REQUIRE(a.ord('g') == 6);
    REQUIRE(a.ord('i') == 7);

    a.add_character('1');
    REQUIRE(a.ord('1') == 0);
    REQUIRE(a.ord('a') == 1);
    REQUIRE(a.ord('b') == 2);
    REQUIRE(a.ord('c') == 3);
    REQUIRE(a.ord('f') == 6);
    REQUIRE(a.ord('i') == 8);

    for (auto& c : a.characters()) {
        REQUIRE(a.chr(a.ord(c)) == c);
    }
}
