/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <ostream>

namespace pur {

template<typename T>
class Optional {
    T val;
    bool is_empty;

 public:
    // deliberately non-explicit
    Optional(const T &val) : val{val}, is_empty{false} {} // NOLINT
    Optional() : val{}, is_empty{true} {}

    Optional(const Optional<T>& oth)
            : val{oth.val}, is_empty{oth.is_empty} {}

    Optional(Optional<T>&& oth) noexcept
            : val{std::move(oth.val)}, is_empty{oth.is_empty} {}

    template<typename U = T>
    Optional& operator=(const Optional<U>& other);

    template<typename U = T>
    Optional& operator=(Optional<U>&& other) noexcept;

    Optional& operator=(const T& value);
    Optional& operator=(T&& value);

    template<typename U>
    friend class Optional;

    bool has_value() const { return !is_empty; }

    const T& value() const { return val; }

    T value_or(const T& def) { if (is_empty) { return def; } return val; }

    operator T() const {  // NOLINT
        return val;
    }

    bool operator==(const Optional& oth) {
        if (is_empty) { return oth.is_empty; }
        return val == oth.val;
    }

    bool operator!=(const Optional& oth) {
        return !operator==(oth);
    }

    friend std::ostream &operator<<(std::ostream &os, const Optional &self) {
        os << "Optional(" << self.val << ", empty: " << self.is_empty << ")";
        return os;
    }
};

template<typename T>
template<typename U>
Optional<T> &Optional<T>::operator=(const Optional<U> &other) {
    is_empty = other.is_empty;
    val = other.val;
    return *this;
}

template<typename T>
template<typename U>
Optional<T>& Optional<T>::operator=(Optional<U> &&other) noexcept {
    is_empty = other.is_empty;
    val = std::move(other.val);
    return *this;
}

template<typename T>
Optional<T> &Optional<T>::operator=(T &&value) {
    is_empty = false;
    val = std::move(value);

    return *this;
}

template<typename T>
Optional<T> &Optional<T>::operator=(const T &value) {
    is_empty = false;
    val = value;

    return *this;
}
}  // namespace pur
