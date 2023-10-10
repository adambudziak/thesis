/*
 * author: Adam Budziak
 */

#include <dfare/dfa/RankedDFA.hpp>
#include <dfare/regexp/from_string.hpp>
#include <iostream>
#include <pur/logger/stream_operators.hpp>
#include "../catch/catch.hpp"

TEST_CASE("RankedDFA ranking") {
    using namespace dfare::dfa;
    using namespace dfare::regexp;

    SECTION("Basic example") {
        auto dfa = RankedDFA{DFA::from_regex(from_string("(a+b)*")), 3};

        using namespace pur::stream_operators;

        REQUIRE(dfa.rank("aaa") == 0);
        REQUIRE(dfa.rank("aba") == 2);
        REQUIRE(dfa.rank("baa") == 4);
        REQUIRE(dfa.rank("bab") == 5);
        REQUIRE(dfa.rank("bbb") == 7);
    }

    SECTION("More complex example") {
        auto dfa = RankedDFA::from_string("Adam_+Ewa__+Alice+Bob__", 5);

        REQUIRE(dfa.rank("Adam_") == 0);
        REQUIRE(dfa.rank("Alice") == 1);
        REQUIRE(dfa.rank("Bob__") == 2);
        REQUIRE(dfa.rank("Ewa__") == 3);

        REQUIRE(!dfa.accepts("Adam"));
        REQUIRE(!dfa.accepts("Bob"));
        REQUIRE(!dfa.accepts("AdEwa"));
        REQUIRE(!dfa.accepts("adam_"));
        REQUIRE(!dfa.accepts("ewa__"));
    }
}

TEST_CASE("RankedDFA unranking") {
    // TODO(abudziak) handle invalid unrank input
    using namespace dfare::dfa;

    SECTION("Basic unranking") {
        auto dfa = RankedDFA::from_string("(a+b)*", 3);

        using namespace pur::stream_operators;
        REQUIRE(dfa.unrank(0) == "aaa");
        REQUIRE(dfa.unrank(5) == "bab");
        REQUIRE(dfa.unrank(7) == "bbb");
    }

    SECTION("More complex example") {
        auto dfa = RankedDFA::from_string("Adam_+Ewa__+Alice+Bob__", 5);

        REQUIRE(dfa.unrank(0) == "Adam_");
        REQUIRE(dfa.unrank(1) == "Alice");
        REQUIRE(dfa.unrank(2) == "Bob__");
        REQUIRE(dfa.unrank(3) == "Ewa__");
    }
}

TEST_CASE("Rank-unrank") {
    using namespace dfare::dfa;
    auto dfa = RankedDFA::from_string("(a+b+c)*", 3);

    for (size_t i = 0; i < dfa.max_rank; ++i) {
        REQUIRE(dfa.rank(dfa.unrank(i)) == i);
    }
}
