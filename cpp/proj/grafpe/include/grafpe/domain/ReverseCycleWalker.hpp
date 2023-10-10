/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/domain/utils.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include "MultiRoundsDT.hpp"

#include <grafpe/config.hpp>

namespace grafpe::domain::targeting {

/**
 * A PoC implementation of Reverse 2-cycle Walker.
 * @tparam PRNG_T
 */
template<typename Crypter_T,
         typename PRNG_T = aes::openssl::AesCtr128PRNG>
class ReverseCycleWalker: public MultiRoundsDT<Crypter_T> {
    using Self = ReverseCycleWalker<Crypter_T, PRNG_T>;

 public:
    using  value_type = typename Crypter_T::value_type;
    using    key_type = typename PRNG_T::key_type;
    using     iv_type = typename PRNG_T::iv_type;
    using dom_fn_type = std::function<bool(const value_type&)>;


    ReverseCycleWalker(vec_u8_t swap_seed, dom_fn_type domain_fn)
      : swap_fn_seed {std::move(swap_seed)}, in_domain {std::move(domain_fn)}
      {
#ifndef EXTENDED_RC_WALKER
          static_assert(std::is_integral_v<value_type>, "RCW supports only number-based FPE");
#endif
      }

    ReverseCycleWalker(vec_u8_t swap_seed, dom_fn_type domain_fn, uint64_t rounds)
    : MultiRoundsDT<Crypter_T>(rounds),
      swap_fn_seed {std::move(swap_seed)}, in_domain {std::move(domain_fn)} {
#ifndef EXTENDED_RC_WALKER
        static_assert(std::is_integral_v<value_type>, "RCW supports only number-based FPE");
#endif
    }

    static std::shared_ptr<Self> create_ptr(vec_u8_t swap_seed, dom_fn_type domain_fn, uint64_t rounds);

 private:
    vec_u8_t swap_fn_seed;
    dom_fn_type in_domain;

    value_type one_round(const Crypter_T& crypter, const value_type& x,
            const tweak_t& tweak, uint64_t round_num) const;

    value_type encrypt_impl(const Crypter_T& crypter, value_type x,
                            const tweak_t& tweak, uint64_t rounds) const override;

    value_type decrypt_impl(const Crypter_T& crypter, value_type x,
                            const tweak_t& tweak, uint64_t rounds) const override;
};


template <typename Crypter_T, typename PRNG_T>
auto ReverseCycleWalker<Crypter_T, PRNG_T>::one_round(
        const Crypter_T& crypter, const value_type& x, const tweak_t& tweak, uint64_t round_num)
const -> value_type {
    tweak_t tweak_ {*tweak.data() + round_num};
    value_type y = crypter.encrypt(x, tweak_);
    value_type z = crypter.decrypt(x, tweak_);

#ifdef EXTENDED_RC_WALKER
    auto swap_fn = detail::create_boolPRF<PRNG_T, value_type>(swap_fn_seed, tweak_);
#else
    auto swap_fn = detail::create_boolPRF<PRNG_T>(swap_fn_seed, tweak_, crypter.size());
#endif

    if (in_domain(y) && !in_domain(z) && !in_domain(crypter.encrypt(y, tweak_))) {
        return swap_fn(x) ? y : x;
    } else if (!in_domain(y) && in_domain(z) && !in_domain(crypter.decrypt(z, tweak_))) {
        return swap_fn(z) ? z : x;
    } else {
        return x;
    }
}


template <typename Crypter_T, typename PRNG_T>
auto ReverseCycleWalker<Crypter_T, PRNG_T>::encrypt_impl(
        const Crypter_T& crypter, value_type x, const tweak_t& tweak, uint64_t rounds)
const -> value_type {
    for (uint64_t round = 0; round < rounds; ++round) {
        x = one_round(crypter, x, tweak, round);
    }
    return x;
}


template <typename Crypter_T, typename PRNG_T>
auto ReverseCycleWalker<Crypter_T, PRNG_T>::decrypt_impl(
        const Crypter_T& crypter, value_type x, const tweak_t& tweak, uint64_t rounds)
const -> value_type {
    for (uint64_t round = rounds; round > 0; --round) {
        x = one_round(crypter, x, tweak, round-1);
    }
    return x;
}

template<typename Crypter_T, typename PRNG_T>
auto
ReverseCycleWalker<Crypter_T, PRNG_T>::create_ptr(
        vec_u8_t swap_seed, ReverseCycleWalker::dom_fn_type domain_fn, uint64_t rounds)
-> std::shared_ptr<Self> {
    return std::make_shared<Self>(std::move(swap_seed), std::move(domain_fn), rounds);
}
}  // namespace grafpe::domain::targeting
