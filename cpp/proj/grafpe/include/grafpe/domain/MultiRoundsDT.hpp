/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/crypt/utils.hpp>

namespace grafpe::domain::targeting {

template <typename Crypter_T>
class MultiRoundsDT {
    using value_type = typename Crypter_T::value_type;

 public:
    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, const value_type& x) const {
        return encrypt_impl(crypter, x, {}, default_rounds_cnt);
    }

    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, const value_type& x) const {
        return decrypt_impl(crypter, x, {}, default_rounds_cnt);
    }

    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, const value_type& x, uint64_t rounds) const {
        return encrypt_impl(crypter, x, {}, rounds);
    }

    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, const value_type& x, uint64_t rounds) const {
        return decrypt_impl(crypter, x, {}, rounds);
    }

    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, const value_type& x, const tweak_t& tweak) const {
        return encrypt_impl(crypter, x, tweak, default_rounds_cnt);
    }

    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, const value_type& x, const tweak_t& tweak) const {
        return decrypt_impl(crypter, x, tweak, default_rounds_cnt);
    }

    [[nodiscard]]
    value_type encrypt(const Crypter_T& crypter, const value_type& x,
            const tweak_t& tweak, uint64_t round) const {
        return encrypt_impl(crypter, x, tweak, round);
    }

    [[nodiscard]]
    value_type decrypt(const Crypter_T& crypter, const value_type& x,
            const tweak_t& tweak, uint64_t round) const {
        return decrypt_impl(crypter, x, tweak, round);
    }

    virtual MultiRoundsDT<Crypter_T>& set_rounds_cnt(uint64_t rounds) {
        default_rounds_cnt = rounds;
        return *this;
    }

    [[nodiscard]]
    virtual uint64_t get_rounds_cnt() const {
        return default_rounds_cnt;
    }

    ~MultiRoundsDT() = default;

 protected:
    MultiRoundsDT() = default;
    explicit MultiRoundsDT(uint64_t rounds_cnt): default_rounds_cnt { rounds_cnt } {   }

    uint64_t default_rounds_cnt {0};

    virtual value_type encrypt_impl(const Crypter_T& crypter, value_type x,
            const tweak_t& tweak, uint64_t round) const = 0;

    virtual value_type decrypt_impl(const Crypter_T& crypter, value_type x,
            const tweak_t& tweak, uint64_t round) const = 0;
};

}  // namespace grafpe::domain::targeting
