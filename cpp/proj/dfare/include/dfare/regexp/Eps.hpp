/*
 * author: Adam Budziak
 */

#pragma once

#include <string>

#include "RegExpBase.hpp"
#include "Empty.hpp"

namespace dfare::regexp {

class Eps: public RegExpBase_<Eps> {
 public:
    explicit operator std::string() const override {
        return "eps";
    }

    regexp_ptr derivative(char prefix) const override {
        return Empty::create();
    }

    bool matches_empty() const override { return true; }

    bool operator==(const Eps& other) const { return true; }
};
}  // namespace dfare::regexp
