/*
 * author: Adam Budziak
 */

#include <dfare/dfa/DFA.hpp>
#include <dfare/regexp/from_string.hpp>
#include <iostream>
#include <sstream>
#include "../catch/catch.hpp"

TEST_CASE("Basic runs") {
    using namespace dfare::dfa;
    using namespace dfare::regexp;
    auto dfa = DFA::from_regex(from_string("a(b+c)*d"));

    std::ostringstream oss;

    const auto test = [&oss, &dfa](auto& input, auto& expected) {
        oss.str("");
        oss.clear();
        oss << dfa.states.at(dfa.run(input));
        REQUIRE(oss.str() == expected);
    };

    test("a", "((b+c))*d");
    test("ab", "((b+c))*d");
    test("abcbc", "((b+c))*d");
    test("ad", "eps");
    test("abcbcbd", "eps");
    test("adbd", "Null");
    test("e", "Null");
    test("", "a((b+c))*d");
}

TEST_CASE("Test DFA accepting regexps") {
    using namespace dfare::dfa;
    using namespace dfare::regexp;
    SECTION("Simple regex") {
        auto dfa = DFA::from_regex(from_string("a(b+c)*d"));


        REQUIRE(dfa.accepts("ad"));
        REQUIRE(dfa.accepts("abcd"));
        REQUIRE(dfa.accepts("abcbcd"));
        REQUIRE(dfa.accepts("accd"));
        REQUIRE(dfa.accepts("abbd"));
        REQUIRE(!dfa.accepts(""));
        REQUIRE(!dfa.accepts("a"));
        REQUIRE(!dfa.accepts("d"));
        REQUIRE(!dfa.accepts("bcd"));
        REQUIRE(!dfa.accepts("abad"));
        REQUIRE(!dfa.accepts("adbd"));
    }
    SECTION("More complicated") {
        auto dfa = DFA::from_regex(from_string("Adam_+Ewa__+Alice+Bob__"));

        REQUIRE(dfa.accepts("Adam_"));
        REQUIRE(dfa.accepts("Ewa__"));
        REQUIRE(dfa.accepts("Alice"));
        REQUIRE(dfa.accepts("Bob__"));

        REQUIRE(!dfa.accepts("Adam"));
        REQUIRE(!dfa.accepts("adam_"));
        REQUIRE(!dfa.accepts("_ewa_"));
        REQUIRE(!dfa.accepts("alic"));
        REQUIRE(!dfa.accepts("lice"));
        REQUIRE(!dfa.accepts("BoB__"));
    }

    SECTION("a[b-c]*d") {
        auto dfa = DFA::from_regex(from_string("a[b-c]*d"));

        REQUIRE(dfa.accepts("ad"));
        REQUIRE(dfa.accepts("abd"));
        REQUIRE(dfa.accepts("acd"));
        REQUIRE(dfa.accepts("abcd"));
        REQUIRE(dfa.accepts("acbd"));
        REQUIRE(dfa.accepts("acbbcd"));
        REQUIRE(dfa.accepts("acbbcbcd"));

        REQUIRE(!dfa.accepts("a"));
        REQUIRE(!dfa.accepts("d"));
        REQUIRE(!dfa.accepts("bd"));
        REQUIRE(!dfa.accepts("ab"));
        REQUIRE(!dfa.accepts("adc"));
        REQUIRE(!dfa.accepts("adb"));
    }
}
