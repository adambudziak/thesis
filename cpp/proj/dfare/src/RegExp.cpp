/*
 * author: Adam Budziak
 */

#include <string>
#include <iostream>

#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Empty.hpp>
#include <dfare/regexp/Eps.hpp>

namespace dfare::regexp {

RegExp::operator std::string() const {
    return {value};
}

regexp_ptr RegExp::derivative(char prefix) const {
    return prefix == value ? Eps::create() : Empty::create();
}

bool RegExp::operator==(const RegExp &other) const {
    return value == other.value;
}

bool RegExp::matches_empty() const {
    return false;
}

}  // namespace dfare::regexp
