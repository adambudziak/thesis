/*
 * author: Adam Budziak
 */

#pragma once

#include <functional>
#include <utility>

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/prng/PRNG.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/domain/utils.hpp>
#include <grafpe/domain/MultiRoundsDT.hpp>
#include <grafpe/prf/RangeBoolPRF.hpp>

#include <grafpe/config.hpp>
#include <grafpe/prf/FastBoolPRF.hpp>

namespace grafpe::domain::targeting {

struct CS_Seed {

    template<typename C1, typename C2>
    CS_Seed(C1&& swp_seed, C2&& dir_seed)
    : dir_fn_seed{swp_seed.begin(), swp_seed.end()},
      swp_fn_seed{dir_seed.begin(), dir_seed.end()} {
//          static_assert(std::is_same_v<typename C1::value_type, uint8_t>
//                     && std::is_same_v<typename C2::value_type, uint8_t>);
      }

    vec_u8_t dir_fn_seed;
    vec_u8_t swp_fn_seed;
};

/**
 * A PoC class implementing basic CycleSlicer (CS) - an algorithm
 * used for Domain Targeting (DT) and Domain Completion (DC)
 *
 * For DT it's an alternative for CycleWalker (CW). CS should
 * perform better for bigger subsets (ie. in the area, where CW
 * performs poorly.
 *
 * CS can be also used for other purposes, which can be parametrized
 * by custom definitions of the Inclusion Function.
 */
template<typename Crypter_T,
         typename PRNG_T = aes::openssl::AesCtr128PRNG>
class CycleSlicer: public MultiRoundsDT<Crypter_T> {
    using Self = CycleSlicer<Crypter_T, PRNG_T>;

 public:
    using value_type = typename Crypter_T::value_type;
    using   key_type = typename PRNG_T::key_type;
    using    iv_type = typename PRNG_T::iv_type;
    using incl_fn_type = std::function<bool(const value_type&, const value_type&)>;


    CycleSlicer(CS_Seed seed_, incl_fn_type incl_fn)
    : seed { std::move(seed_) }, incl_fn{std::move(incl_fn)},
      dir_fn { initialize_bool_prf(seed.dir_fn_seed)},
      swp_fn { initialize_bool_prf(seed.swp_fn_seed)}{
#ifndef EXTENDED_CYCLE_SLICER
        static_assert(std::is_integral_v<value_type>);
#endif
    }

    CycleSlicer(CS_Seed seed,
                incl_fn_type incl_func, uint64_t default_rounds_cnt)
    : MultiRoundsDT<Crypter_T>(default_rounds_cnt),
      seed { std::move(seed) }, incl_fn {std::move(incl_func)},
      dir_fn { initialize_bool_prf(seed.dir_fn_seed)},
      swp_fn { initialize_bool_prf(seed.swp_fn_seed)}{
#ifndef EXTENDED_CYCLE_SLICER
        static_assert(std::is_integral_v<value_type>);
#endif
    }

    static std::shared_ptr<Self> create_ptr(CS_Seed seed, incl_fn_type incl_fn, uint64_t rounds);

 private:
    CS_Seed seed;
    incl_fn_type incl_fn;
    mutable FastBoolPRF<typename PRNG_T::cipher_type, value_type> dir_fn;
    mutable FastBoolPRF<typename PRNG_T::cipher_type, value_type> swp_fn;

    FastBoolPRF<typename PRNG_T::cipher_type, value_type>
    initialize_bool_prf(const vec_u8_t& seed) const {
        typename PRNG_T::key_type key {};
        typename PRNG_T::iv_type   iv {};
        generate_key_iv<typename PRNG_T::ctx_type>(seed, key, iv);
        return FastBoolPRF<typename PRNG_T::cipher_type, value_type>{key, iv};
    }


    value_type one_round(const Crypter_T& crypter, const value_type& x,
            const tweak_t& tweak, uint64_t round_num) const;

    value_type encrypt_impl(const Crypter_T& crypter, value_type x,
                            const tweak_t& tweak, uint64_t rounds) const override;

    value_type decrypt_impl(const Crypter_T& crypter, value_type x,
                            const tweak_t& tweak, uint64_t rounds) const override;
};


template <typename Crypter_T, typename PRNG_T>
auto CycleSlicer<Crypter_T, PRNG_T>::encrypt_impl(
        const Crypter_T& crypter, value_type x, const tweak_t& tweak, uint64_t rounds)
const -> value_type {
    for (uint64_t round = 0; round < rounds; ++round) {
        x = one_round(crypter, x, tweak, round);
    }
    return x;
}

template <typename Crypter_T, typename PRNG_T>
auto CycleSlicer<Crypter_T, PRNG_T>::decrypt_impl(
        const Crypter_T& crypter, value_type x, const tweak_t& tweak, uint64_t rounds)
const -> value_type {
    for (auto round = rounds; round > 0; --round) {
        x = one_round(crypter, x, tweak, round-1);
    }
    return x;
}

template <typename Crypter_T, typename PRNG_T>
auto CycleSlicer<Crypter_T, PRNG_T>::one_round(
        const Crypter_T& crypter, const value_type& x, const tweak_t& tweak, uint64_t round_num)
const -> value_type {
    tweak_t tweak_{*tweak.data() + round_num};
    if (dir_fn(x, tweak_)) {
        auto xn = crypter.encrypt(x, tweak_);
        if (!dir_fn(xn, tweak_) && incl_fn(x, xn) && swp_fn(x, tweak_)) {
            return xn;
        }
    } else {
        auto xp = crypter.decrypt(x, tweak_);
        if (dir_fn(xp, tweak_) && incl_fn(xp, x) && swp_fn(xp, tweak_)) {
            return xp;
        }
    }
    return x;
}

template<typename Crypter_T, typename PRNG_T>
auto CycleSlicer<Crypter_T, PRNG_T>::create_ptr(
        CS_Seed seed, CycleSlicer::incl_fn_type incl_fn, uint64_t rounds)
-> std::shared_ptr<Self>{
    return std::make_shared<Self>(std::move(seed), std::move(incl_fn), rounds);
}

}  // namespace grafpe::domain::targeting
