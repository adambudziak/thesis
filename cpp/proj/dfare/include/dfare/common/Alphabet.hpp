/*
 * author: Adam Budziak
 */

#pragma once

#include <algorithm>
#include <string>
#include <utility>

namespace dfare {

class Alphabet {
    std::string characters_;

 public:
    Alphabet() = default;
    explicit Alphabet(std::string characters):
            characters_{std::move(characters)} {
        std::sort(characters_.begin(), characters_.end());
        characters_.erase(std::unique(characters_.begin(), characters_.end()), characters_.end());
    }

    bool add_character(char c);

    size_t ord(char c) const;
    char chr(std::size_t index) const;

    const std::string& characters() const;

    bool operator==(const Alphabet &rhs) const;

    bool operator!=(const Alphabet &rhs) const;

    decltype(characters_.begin()) begin() noexcept;
    decltype(characters_.end()) end() noexcept;

    decltype(characters_)::const_iterator begin() const noexcept;
    decltype(characters_)::const_iterator end() const noexcept;

    decltype(characters_.cbegin()) cbegin() const noexcept;
    decltype(characters_.cend()) cend() const noexcept;
};

}  // namespace dfare
