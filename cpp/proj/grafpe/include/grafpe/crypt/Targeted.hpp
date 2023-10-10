/*
 * author: Adam Budziak
 */

#pragma once

#include <memory>
#include <grafpe/crypt/Crypter.hpp>
#include <grafpe/common/type_utils.hpp>

namespace grafpe::crypter {

template <typename Targeter_T, typename Crypter_T>
class Targeted : public Base<typename Crypter_T::value_type> {
    using Self = Targeted<Targeter_T, Crypter_T>;
    Targeter_T targeter;
    Crypter_T crypter;

 public:
    using value_type = typename Targeter_T::value_type;

    Targeted(Targeter_T targeter, Crypter_T crypter)
    : targeter { std::move(targeter) }, crypter { std::move(crypter) }
    {  }

 private:
    constexpr value_type encrypt_impl(const value_type& value, const tweak_t& tweak) const override;
    constexpr value_type decrypt_impl(const value_type& value, const tweak_t& tweak) const override;
};

template<typename Targeter_T, typename Crypter_T>
constexpr auto
Targeted<Targeter_T, Crypter_T>::encrypt_impl(const value_type &value, const tweak_t &tweak)
const -> value_type {
    return targeter.encrypt(crypter, value, tweak);
}

template<typename Targeter_T, typename Crypter_T>
constexpr auto
Targeted<Targeter_T, Crypter_T>::decrypt_impl(const value_type &value, const tweak_t &tweak)
const -> value_type {
    return targeter.decrypt(crypter, value, tweak);
}

}  // namespace grafpe::crypter
