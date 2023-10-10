/*
 * author: Adam Budziak
 */

#include <functional>
#include <string>

#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Sum.hpp>

#include "../catch/catch.hpp"


TEST_CASE("Test to-string conversion") {
    using namespace dfare::regexp;
    REQUIRE(std::string {RegExp {'c'}} == std::string {"c"});

    using namespace std::placeholders;
    auto left = RegExp::create('a');
    auto right = RegExp::create('b');
    REQUIRE(std::string {Sum {left, right}} == std::string {"(a+b)"});

    auto regexp = Concat::create(
            Concat::create(RegExp::create('a'), RegExp::create('a')),
            Sum::create(RegExp::create('a'), RegExp::create('b')));

    REQUIRE(std::string {*regexp} == std::string {"aa(a+b)"});
}
