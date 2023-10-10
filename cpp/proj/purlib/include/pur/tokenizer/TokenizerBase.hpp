/*
 * author: Adam Budziak
 */

#pragma once

#include <map>
#include <string>
#include <ostream>

#include <pur/optional/Optional.hpp>

namespace pur {

class TokenizerBase {
 protected:
    using tokens_t = std::map<std::string, pur::Optional<std::string>>;

 public:
    virtual ~TokenizerBase() = default;
    virtual void tokenize(const std::string&) = 0;
    virtual const tokens_t& tokenize(int argc, char **argv) = 0;
    virtual const tokens_t& get_tokens() const = 0;
};
}  // namespace pur
