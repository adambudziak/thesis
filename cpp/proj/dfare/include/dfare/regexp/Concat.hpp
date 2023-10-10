/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <utility>

#include "RegExpBase.hpp"

namespace dfare::regexp {

class Concat: public RegExpBase_<Concat> {
 public:
    const regexp_ptr left, right;
    Concat(regexp_ptr left, regexp_ptr right)
            : left {std::move(left)}, right {std::move(right)} {  }

    bool matches_empty() const override;

    explicit operator std::string() const override;

    regexp_ptr derivative(char prefix) const override;

    bool operator==(const Concat& other) const;

    static regexp_ptr create(regexp_ptr left, regexp_ptr right);

    bool equivalent(const Concat &other) const;
};

}  // namespace dfare::regexp
