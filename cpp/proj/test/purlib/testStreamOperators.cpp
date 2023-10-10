/*
 * author: Adam Budziak
 */

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <pur/logger/stream_operators.hpp>
#include "../catch/catch.hpp"


TEST_CASE("Map stream operators") {
    using namespace pur::stream_operators;

    std::map<char, int> map {{'a', 5}, {'b', 6}, {'c', 7}};
    std::map<int, std::map<char, int>> states {{1, map}, {2, map}};

    std::ostringstream oss;

    oss << map;
    std::string map_str {R"({"a": 5, "b": 6, "c": 7})"};
    REQUIRE(oss.str() == map_str);

    oss.str("");
    oss.clear();
    oss << states;
    REQUIRE(oss.str() == "{1: " + map_str + ", 2: " + map_str + "}");

    std::vector<int> vec {1, 5, 7, 9, 2, 3};
    oss.str("");
    oss.clear();
    oss << vec;
    REQUIRE(oss.str() == "[1, 5, 7, 9, 2, 3]");
}

