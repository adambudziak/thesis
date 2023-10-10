/*
 * author: Adam Budziak
 */

#pragma once

// TODO(abudziak) customizable dictionaries

#include <string>
#include "RegExpBase.hpp"

namespace dfare::regexp {

/**
 * Class holding the simplest value (e.g. one character)
 */
class RegExp : public RegExpBase_<RegExp> {
    char value;

 public:
    bool matches_empty() const override;

    explicit RegExp(char value): value {value} {}
    explicit operator std::string() const override;

    regexp_ptr derivative(char prefix) const override;
    bool operator==(const RegExp& other) const;
};

}  // namespace dfare::regexp
