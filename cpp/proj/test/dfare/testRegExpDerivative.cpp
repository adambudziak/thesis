/*
 * author: Adam Budziak
 */

#include <dfare/regexp/RegExpBase.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <iostream>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>
#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/Closure.hpp>
#include "../catch/catch.hpp"


TEST_CASE("Derivatives") {
    using namespace dfare::regexp;

    SECTION("Eps and Empty") {
        REQUIRE(Eps::create().derivative('a') == Empty{});
        REQUIRE(Eps::create().derivative('c') == Empty{});
        REQUIRE(Empty::create().derivative('1') == Empty{});
    }

    SECTION("One character regexp") {
        REQUIRE(RegExp::create('a').derivative('a') == Eps::create());
        REQUIRE(RegExp::create('a').derivative('b') == Empty::create());
        REQUIRE(RegExp::create('b').derivative('a') == Empty::create());
    }

    SECTION("Sum") {
        REQUIRE(Sum {RegExp::create('a'), RegExp::create('b')}.derivative('a') == Eps::create());
        REQUIRE(Sum {RegExp::create('b'), RegExp::create('a')}.derivative('a') == Eps::create());
        REQUIRE(Sum {RegExp::create('b'), RegExp::create('b')}.derivative('a') == Empty::create());
    }

    SECTION("Concat") {
        REQUIRE(Concat {RegExp::create('a'), RegExp::create('b')}.derivative('a')
                ==
                RegExp::create('b'));
        REQUIRE(Concat {RegExp::create('b'), RegExp::create('b')}.derivative('a')
                ==
                Empty::create());
    }

    SECTION("Closure") {
        REQUIRE(Closure {RegExp::create('a')}.derivative('a')
                ==
                Closure::create(RegExp::create('a')));
        REQUIRE(Closure {RegExp::create('a')}.derivative('b')
                ==
                Empty::create());
    }
}


TEST_CASE("Test derivatives of mixed") {
    using namespace dfare::regexp;
    REQUIRE(Concat::create(Closure::create(RegExp::create('a')), RegExp::create('b')).derivative('b')
            ==
            Eps::create());
}
