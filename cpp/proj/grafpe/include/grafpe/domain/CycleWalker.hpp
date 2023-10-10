/*
 * author: Adam Budziak
 */

#pragma once

#include <functional>
#include <utility>

#include "grafpe/crypt/Grafpe.hpp"

namespace grafpe::domain::targeting {

template <typename Crypter_T>
class CycleWalker {
 public:
    using value_type = typename Crypter_T::value_type;

 private:
    using predicate_t = std::function<bool(const value_type&)>;
    predicate_t predicate;
    using Self = CycleWalker<Crypter_T>;

 public:
    explicit CycleWalker(predicate_t predicate)
        : predicate {std::move(predicate)} { }

    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, const value_type& x) const {
        return encrypt(crypter, x, {});
    }
    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, const value_type& x) const {
        return decrypt(crypter, x, {});
    }

    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, value_type x, const tweak_t& tweak) const;
    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, value_type x, const tweak_t& tweak) const;

    static std::shared_ptr<Self> create_ptr(predicate_t&& predicate);
};

template<typename Crypter_T>
auto
CycleWalker<Crypter_T>::encrypt(const Crypter_T &crypter, value_type x, const tweak_t& tweak)
const -> value_type {
    do {
        x = crypter.encrypt(x, tweak);
    } while (!predicate(x));
    return x;
}

template<typename Crypter_T>
auto
CycleWalker<Crypter_T>::decrypt(const Crypter_T &crypter, value_type x, const tweak_t& tweak)
const -> value_type {
    do {
        x = crypter.decrypt(x, tweak);
    } while (!predicate(x));
    return x;
}

template<typename Crypter_T>
auto CycleWalker<Crypter_T>::create_ptr(predicate_t&& predicate) -> std::shared_ptr<Self> {
    return std::make_shared<Self>(std::move(predicate));
}

}  // namespace grafpe::domain::targeting
