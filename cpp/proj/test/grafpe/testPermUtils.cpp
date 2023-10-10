/*
 * author: Adam Budziak
 */

#include "../catch/catch.hpp"

#include <pur/logger/stream_operators.hpp>
#include <grafpe/permutation/utils.hpp>

TEST_CASE("Permutation validation") {
    using namespace grafpe;

    SECTION("Permutations pass the test") {
        REQUIRE(is_permutation<vec_u32_t>({0, 1, 2, 3, 4}));
        REQUIRE(is_permutation<vec_u32_t>({0, 3, 1, 4, 2}));
        REQUIRE(is_permutation<vec_u32_t>({0}));
        REQUIRE(is_permutation<vec_u32_t>({6, 5, 4, 0, 3, 2, 1}));
    }

    SECTION("Non-permutation vectors fail") {
        WHEN("Any element is >= the size") {
            REQUIRE(!is_permutation<vec_u32_t>({1, 2, 3, 4, 5}));
            REQUIRE(!is_permutation<vec_u32_t>({6, 2, 1, 4, 3}));
        }

        WHEN("Any element is not unique") {
            REQUIRE(!is_permutation<vec_u32_t>({2, 4, 3, 2, 1, 0}));
        }
    }
}

TEST_CASE("Valid permutation is valid") {
    using namespace grafpe;
    REQUIRE(!grafpe::valid_permutation(perm_t{1, 2, 0}).has_value());
}

TEST_CASE("If p[i] == i then permutation is invalid") {
    using namespace grafpe;
    REQUIRE(grafpe::valid_permutation(perm_t{0, 2, 3, 4, 1}).value() == 0);
    REQUIRE(grafpe::valid_permutation(perm_t{2, 1, 4, 3, 0}).value() == 1);
    REQUIRE(grafpe::valid_permutation(perm_t{3, 4, 2, 1, 0}).value() == 2);
    REQUIRE(grafpe::valid_permutation(perm_t{2, 4, 1, 3, 0}).value() == 3);
    REQUIRE(grafpe::valid_permutation(perm_t{2, 3, 1, 0, 4}).value() == 4);
}

TEST_CASE("If p[p[i]] == i then permutation is invalid") {
    using namespace grafpe;
    REQUIRE(grafpe::valid_permutation(perm_t{4, 3, 2, 1, 0}).value() == 0);
    REQUIRE(grafpe::valid_permutation(perm_t{2, 3, 4, 1, 0}).value() == 1);
    REQUIRE(grafpe::valid_permutation(perm_t{3, 0, 4, 1, 2}).value() == 2);
    REQUIRE(grafpe::valid_permutation(perm_t{2, 0, 1, 4, 3}).value() == 3);
    // should never return the last element since it would check
    // the first one in the pair earlier
}

TEST_CASE("Permutation inverse") {
    using namespace grafpe;
    auto uut = inverse_permutation<perm_t>;
    REQUIRE(uut({0, 1, 2, 3, 4}) == perm_t{0, 1, 2, 3, 4});
    REQUIRE(uut({0, 4, 1, 2, 3}) == perm_t{0, 2, 3, 4, 1});

    // inverse(inverse(P)) == P
    perm_t p {0, 2, 3, 1, 6, 5, 4};
    REQUIRE(uut(uut(p)) == p);
    p = {9, 5, 8, 1, 0, 2, 7, 4, 3, 6};
    REQUIRE(uut(uut(p)) == p);
}
