/*
 * author: Adam Budziak
 */

#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>
#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/Closure.hpp>
#include "../catch/catch.hpp"
using namespace dfare::regexp;

TEST_CASE("Basic regexps") {
    REQUIRE(Empty::create().equivalent(Empty::create()));
    REQUIRE(Eps::create().equivalent(Eps::create()));
    REQUIRE(!RegExp::create('a').equivalent(Eps::create()));
    REQUIRE(!RegExp::create('a').equivalent(Empty::create()));
    REQUIRE(!RegExp::create('a').equivalent(RegExp::create('b')));
    REQUIRE(RegExp::create('a').equivalent(RegExp::create('a')));
}

TEST_CASE("Sums equivalency") {
    auto a = RegExp::create('a');
    auto b = RegExp::create('b');
    auto c = RegExp::create('c');
    auto empty = Empty::create();

    REQUIRE(Sum::create(a, a).equivalent(a));
    REQUIRE(Sum::create(empty, a).equivalent(a));
    REQUIRE(Sum::create(a, b).equivalent(Sum::create(a, b)));
    REQUIRE(Sum::create(a, b).equivalent(Sum::create(b, a)));
    REQUIRE(Sum::create(Sum::create(a, b), c).equivalent(Sum::create(a, Sum::create(b, c))));
    REQUIRE(Sum::create(a, Sum::create(b, c)).equivalent(Sum::create(Sum::create(a, b), c)));
}

TEST_CASE("Concats equivalency") {
    auto a = RegExp::create('a');
    auto b = RegExp::create('b');
    auto c = RegExp::create('c');
    auto empty = Empty::create();
    auto eps = Eps::create();

    REQUIRE(!Concat::create(a, b).equivalent(Sum::create(a, b)));
    REQUIRE(!Concat::create(a, b).equivalent(Concat::create(b, a)));
    REQUIRE(Concat::create(Concat::create(a, b), c).equivalent(Concat::create(a, Concat::create(b, c))));
    REQUIRE(Concat::create(empty, a).equivalent(empty));
    REQUIRE(Concat::create(a, empty).equivalent(empty));
    REQUIRE(Concat::create(a, eps).equivalent(a));
    REQUIRE(Concat::create(eps, a).equivalent(a));
}

TEST_CASE("Closure equivalency") {
    auto a = RegExp::create('a');
    auto eps = Eps::create();
    auto empty = Empty::create();

    REQUIRE(Closure::create(Closure::create(a)).equivalent(Closure::create(a)));
    REQUIRE(Closure::create(eps).equivalent(eps));
    REQUIRE(Closure::create(empty).equivalent(eps));
}
