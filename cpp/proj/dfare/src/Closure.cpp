/*
 * author: Adam Budziak
 */

#include <string>

#include <dfare/regexp/Closure.hpp>
#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Empty.hpp>
#include <dfare/regexp/Eps.hpp>

namespace dfare::regexp {

Closure::operator std::string() const {
    using namespace std::string_literals;
    if (auto c = dynamic_cast<const RegExp*>(value.get()); c) {
        return std::string {*value} + "*";
    }
    return "("s + std::string {*value} + ")*";
}

regexp_ptr Closure::derivative(char prefix) const {
    return Concat::create(
        value.derivative(prefix),
        Closure::create(value));
}

bool Closure::matches_empty() const {
    return true;
}

bool Closure::operator==(const Closure& other) const {
    return value == other.value;
}


regexp_ptr Closure::create(regexp_ptr value) {
    if (dynamic_cast<const Closure*>(value.get())) { return value; }
    if (dynamic_cast<const Eps*>(value.get())) { return value; }
    if (dynamic_cast<const Empty*>(value.get())) { return Eps::create(); }
    return RegExpBase_<Closure>::create(value);
}

}  // namespace dfare::regexp
