/*
 * author: Adam Budziak
 */

#include <vector>

#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/permutation/PermGen.hpp>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/permutation/PermGen_Compat.hpp>
#include <grafpe/common/print_utils.hpp>

#include "../catch/catch.hpp"


TEST_CASE("Permutation generator generates permutations") {
    using namespace grafpe::permutation;
    using namespace grafpe;

    Generator<DummyPRNG> permGen{DummyPRNG{}};
    for (int test_cnt = 0; test_cnt < 100; ++test_cnt) {
        permGen.create_permutation(100);
//        REQUIRE(is_permutation(permGen.create_permutation(100)));
    }
}


TEST_CASE("Compat-generator generates permutations") {
    using namespace grafpe::permutation;
    using namespace grafpe;

    Generator_Compat<DummyPRNG> permGen{DummyPRNG{}};

    for (int test_cnt = 0; test_cnt < 100; ++test_cnt) {
        REQUIRE(is_permutation(permGen.create_permutation(100)));
    }
}
