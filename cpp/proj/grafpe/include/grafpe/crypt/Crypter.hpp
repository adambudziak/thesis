/*
 * author: Adam Budziak
 */

#pragma once


#include "../common/type_utils.hpp"

namespace grafpe::crypter {

template <typename Value_T>
class Base {
 public:
    using value_type = Value_T;
    [[nodiscard]]
    value_type encrypt(const value_type& value) const {
        return encrypt_impl(value, {});
    }

    [[nodiscard]]
    value_type decrypt(const value_type& value) const {
        return decrypt_impl(value, {});
    }

    [[nodiscard]]
    value_type encrypt(const value_type& value, const tweak_t& tweak) const {
        return encrypt_impl(value, tweak);
    }

    [[nodiscard]]
    value_type decrypt(const value_type& value, const tweak_t& tweak) const {
        return decrypt_impl(value, tweak);
    }

 protected:
    Base() = default;
    virtual value_type encrypt_impl(const value_type &value, const tweak_t& tweak) const = 0;
    virtual value_type decrypt_impl(const value_type &value, const tweak_t& tweak) const = 0;
    virtual ~Base() = default;
};

template <template<typename ...> class Crypter, typename ...Args>
auto create(Args&&... args) {
    return std::make_shared<decltype(Crypter{std::forward<Args>(args)...})>(std::forward<Args>(args)...);
}

}  // namespace grafpe::crypter

