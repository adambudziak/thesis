/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <vector>
#include <utility>

#include "RegExpBase.hpp"

namespace dfare::regexp {

class Sum : public RegExpBase_<Sum> {
 public:
    const regexp_ptr left, right;
    Sum(regexp_ptr left, regexp_ptr right):
            left {std::move(left)}, right {std::move(right)} {}

    explicit operator std::string() const override;

    regexp_ptr derivative(char prefix) const override;

    bool matches_empty() const override;

    bool operator==(const Sum& other) const;

    bool equivalent(const Sum &other) const;

    static regexp_ptr create(regexp_ptr left, regexp_ptr right);
};

}  // namespace dfare::regexp
