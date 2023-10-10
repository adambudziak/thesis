/*
 * author: Adam Budziak
 */

#include <vector>
#include <bitset>
#include <iostream>
#include <map>

#include <pur/containers/bitset.hpp>
#include <pur/containers/bitset_view.hpp>

#include "../catch/catch.hpp"

TEST_CASE("Test bitset") {
    std::vector<uint8_t> nums {0, 1, 2, 3, 4, 5, 6, 7};
    auto bitset = pur::bitset{nums, nums.size() * 8};

    std::map<size_t, bool> expected {
            {0, false},
            {8, true},
            {55, false},
            {57, true},
            {58, true},
            {63, false}
    };

    for (auto[i, r] : expected) {
        REQUIRE(bitset[i] == r);
    }

    REQUIRE(bitset.size() == 64);
}

TEST_CASE("Test bitset view") {
    std::vector<uint8_t> nums { 0, 1, 2, 3, 4, 5, 6, 7};
    auto bit_view = pur::bitset_view{nums};

    std::map<size_t, bool> expected {
            {0, false},
            {8, true},
            {55, false},
            {57, true},
            {58, true},
            {63, false}
    };

    for (auto[i, r] : expected) {
        REQUIRE(bit_view[i] == r);
    }

    REQUIRE(bit_view.size() == 64);
}
