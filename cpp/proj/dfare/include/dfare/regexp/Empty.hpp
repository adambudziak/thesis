/*
 * author: Adam Budziak
 */

#pragma once

#include <string>

#include "RegExpBase.hpp"

namespace dfare::regexp {

class Empty: public RegExpBase_<Empty> {
 public:
    explicit operator std::string() const override {
        return "Null";
    }

    regexp_ptr derivative(char prefix) const override {
        return create();
    }

    bool matches_empty() const override {
        return false;
    }

    bool operator==(const Empty& other) const { return true; }
};

}  // namespace dfare::regexp
