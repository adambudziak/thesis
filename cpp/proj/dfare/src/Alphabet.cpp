/*
 * author: Adam Budziak
 */

#include <string>

#include <dfare/common/Alphabet.hpp>

namespace dfare {

size_t Alphabet::ord(char c) const {
    auto iter = std::find(begin(), end(), c);
    return static_cast<size_t>(iter - begin());
}

char Alphabet::chr(std::size_t index) const {
    return characters_.at(index);
}

bool Alphabet::add_character(char c) {
    auto pos = std::lower_bound(characters_.begin(), characters_.end(), c);
    if (pos != characters_.end() && *pos == c) { return false; }
    characters_.insert(pos, c);
    return true;
}

const std::string &Alphabet::characters() const {
    return characters_;
}

bool Alphabet::operator==(const Alphabet &rhs) const {
    return characters_ == rhs.characters_;
}

bool Alphabet::operator!=(const Alphabet &rhs) const {
    return !(rhs == *this);
}

auto Alphabet::begin() noexcept -> decltype(characters_.begin()) {
    return characters_.begin();
}

auto Alphabet::end() noexcept -> decltype(characters_.end()) {
    return characters_.end();
}

auto Alphabet::begin() const noexcept -> decltype(characters_)::const_iterator {
    return characters_.begin();
}

auto Alphabet::end() const noexcept -> decltype(characters_)::const_iterator {
    return characters_.end();
}

auto Alphabet::cbegin() const noexcept -> decltype(characters_.cbegin()) {
    return characters_.cbegin();
}

auto Alphabet::cend() const noexcept -> decltype(characters_.cend()) {
    return characters_.cend();
}
}  // namespace dfare
