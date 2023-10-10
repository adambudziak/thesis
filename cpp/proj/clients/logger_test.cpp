/*
 * author: Adam Budziak
 */

//#include <pur/logger/Logger.hpp>
#include <pur/logger/stream_operators.hpp>
#include <iostream>
#include <vector>
#include <map>

using namespace pur;

int main () {
//    auto logger = logger::Logger::createNonColored(std::cout << std::boolalpha);

//    using stream_operators::operator<<;
    using namespace stream_operators;

//    std::vector a { 1, 2, 3, 4, 5 };
//    std::vector a {'a', 'b', 'c', 'd'};
    std::map<std::string, int> a { {"a", 5}, {"b", 10} };
//    std::vector a {"a", "b", "c", "d"};

    std::cout << a << '\n';

//    logger.info(5, "hejka", 3.14, true, a);
}