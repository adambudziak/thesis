/*
 * author: Adam Budziak
 */

#pragma once

#include <vector>
#include <memory>
#include "Crypter.hpp"

namespace grafpe::crypter {

template<typename Crypter_T, typename Value_T>
class CastCrypter: public Base<Value_T> {
    using crypter_value_t = typename Crypter_T::value_type;
    Crypter_T m_crypter;

 public:
    using value_type = Value_T;

    explicit CastCrypter(Crypter_T crypter): m_crypter { std::move(crypter) } { }
    ~CastCrypter() = default;

    crypter_value_t cast(const value_type& value) const {
        return cast_impl(value);
    }

    value_type uncast(const crypter_value_t& value) const {
        return uncast_impl(value);
    }

 protected:
    value_type encrypt_impl(const value_type &value, const tweak_t &tweak) const final {
        return uncast_impl(m_crypter.encrypt(cast_impl(value), tweak));
    }

    value_type decrypt_impl(const value_type &value, const tweak_t &tweak) const final {
        return uncast_impl(m_crypter.decrypt(cast_impl(value), tweak));
    }

    virtual crypter_value_t cast_impl(const value_type &value) const = 0;
    virtual value_type uncast_impl(const crypter_value_t &value) const = 0;
};

}

