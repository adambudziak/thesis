/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <utility>

namespace dfare {

class invalid_regexp_string : public std::exception {
    std::string regexp_string;

 public:
    explicit invalid_regexp_string(std::string regexp_string)
    : regexp_string { std::move(regexp_string) } {  }

    const char* what() const noexcept override {
        return ("Given string is not a valid regexp: " + regexp_string).c_str();
    }
};

}  // namespace dfare
