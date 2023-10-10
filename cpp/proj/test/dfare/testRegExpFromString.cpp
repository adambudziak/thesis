/*
 * author: Adam Budziak
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include <dfare/regexp/from_string.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Closure.hpp>
#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>
#include <dfare/exceptions.hpp>
#include <dfare/common/Alphabet.hpp>

#include "../catch/catch.hpp"

using namespace dfare::regexp;
using test_data = std::pair<regexp_ptr, std::string>;

TEST_CASE("Test reg exp from string") {
    std::vector<test_data> test_suite {
//            {RegExp::create('a'), "(aa*bb*+bb*aa*)*"},
            {RegExp::create('a'), "a"},
            {RegExp::create('b'), "b"},
            {Concat::create(RegExp::create('a'), RegExp::create('a')), "aa"},
            {
                Concat::create(
                    RegExp::create('a'),
                    Concat::create(
                        RegExp::create('b'),
                        Concat::create(
                            RegExp::create('c'),
                            Concat::create(
                                RegExp::create('d'),
                                Concat::create(
                                    Closure::create(
                                        Sum::create(
                                            RegExp::create('a'),
                                            Sum::create(
                                                Concat::create(RegExp::create('b'), RegExp::create('e')),
                                                RegExp::create('c')))),
                                    Concat::create(
                                        RegExp::create('a'),
                                        RegExp::create('a')
                                    )
                                )
                            )
                        )
                    )
                )
                ,
                "abcd(a+be+c)*aa"
            },
    };

    for (auto& [expected, input] : test_suite) {
        REQUIRE(expected == from_string(input));
    }
}


TEST_CASE("Derivatives from string") {
    auto regex = from_string("a*b (a*b + b*c)*");
    std::cout << regex.derivative('b') << std::endl;
}

TEST_CASE("Invalid regexp strings") {
    REQUIRE_THROWS_AS(from_string("a++b"), dfare::invalid_regexp_string);
    REQUIRE_THROWS_AS(from_string("a+*b"), dfare::invalid_regexp_string);
    REQUIRE_THROWS_AS(from_string("a+*"), dfare::invalid_regexp_string);
    REQUIRE_THROWS_AS(from_string("*a"), dfare::invalid_regexp_string);
    REQUIRE_THROWS_AS(from_string("a("), dfare::invalid_regexp_string);
    REQUIRE_THROWS_AS(from_string("a)b"), dfare::invalid_regexp_string);
}

TEST_CASE("Get alphabet") {
    using namespace dfare;
    REQUIRE(Alphabet{"(a*b(a*b + b*c))"} == Alphabet{" ()+*abc"});
    REQUIRE(Alphabet{"batlk1239*+vcxlkzj"} == Alphabet{"+*1239abcjkltvxz"});
}
