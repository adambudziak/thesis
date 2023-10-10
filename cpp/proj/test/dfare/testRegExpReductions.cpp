/*
 * author: Adam Budziak
 */

#include <dfare/regexp/Closure.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>
#include <dfare/regexp/Concat.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Closure reductions") {
    using namespace dfare::regexp;
    auto closure = Closure::create(Closure::create(RegExp::create('a')));
    REQUIRE(closure == Closure::create(RegExp::create('a')));
    REQUIRE(Closure::create(Eps::create()) == Eps::create());
    REQUIRE(Closure::create(Empty::create()) == Eps::create());
}

TEST_CASE("Sum reductions") {
    using namespace dfare::regexp;
    auto sum = Sum::create(RegExp::create('a'), RegExp::create('a'));
    REQUIRE(sum == RegExp::create('a'));
    sum = Sum::create(Empty::create(), Eps::create());
    REQUIRE(sum == Eps::create());
}

TEST_CASE("Concat reductions") {
    using namespace dfare::regexp;
    auto concat = Concat::create(Empty::create(), RegExp::create('a'));
    REQUIRE(concat == Empty::create());
    concat = Concat::create(Eps::create(), RegExp::create('b'));
    REQUIRE(concat == RegExp::create('b'));
}

TEST_CASE("Concats are right-to-left") {
    using namespace dfare::regexp;
    auto concat = Concat::create(
        Concat::create(RegExp::create('a'), RegExp::create('b')),
        RegExp::create('c'));
    REQUIRE(concat
            ==
            Concat::create(
                    RegExp::create('a'),
                    Concat::create(RegExp::create('b'), RegExp::create('c'))));
}
