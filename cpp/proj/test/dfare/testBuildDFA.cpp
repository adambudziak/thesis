/*
 * author: Adam Budziak
 */

#include "../catch/catch.hpp"

#include <dfare/dfa/DFA.hpp>
#include <dfare/regexp/from_string.hpp>
#include <iostream>

TEST_CASE("Test build DFA") {
    using namespace dfare::dfa;
    using namespace dfare;

    auto dfa = DFA::from_regex(regexp::from_string("ab+ac"));
    std::cout << dfa << std::endl;
}
