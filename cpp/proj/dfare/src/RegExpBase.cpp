/*
 * author: Adam Budziak
 */

#include <string>

#include <dfare/regexp/RegExpBase.hpp>

namespace dfare::regexp {

bool regexp_ptr::operator==(const regexp_ptr &other) const {
    return *ptr == *(other.ptr);
}

bool regexp_ptr::operator==(const RegExpBase& other) const {
    return *ptr == other;
}

regexp_ptr regexp_ptr::derivative(char prefix) const {
    return ptr->derivative(prefix);
}

bool regexp_ptr::equivalent(const regexp_ptr &other) const {
    return ptr->equivalent(*other.ptr);
}

bool regexp_ptr::equivalent(const RegExpBase &other) const {
    return ptr->equivalent(other);
}

bool regexp_ptr::matches_empty() const {
    return ptr->matches_empty();
}

std::ostream& operator<<(std::ostream &os, const regexp_ptr &p) {
    os << std::string{*p.ptr};
    return os;
}

regexp_ptr::operator std::string() {
    return std::string{*ptr};
}

}  // namespace dfare::regexp
