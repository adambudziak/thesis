/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <utility>

#include "RegExpBase.hpp"

namespace dfare::regexp {

class Closure: public RegExpBase_<Closure> {
 public:
    explicit Closure(regexp_ptr value): value{std::move(value)} {  }
    const regexp_ptr value;

    explicit operator std::string() const override;
    regexp_ptr derivative(char prefix) const override;
    bool matches_empty() const override;

    bool operator==(const Closure& other) const;
    static regexp_ptr create(regexp_ptr value);
};

}  // namespace dfare::regexp
