/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/crypt/utils.hpp>
#include <grafpe/prf/RangeBoolPRF.hpp>

#include <GrafpeConfig.hpp>
#include <grafpe/prf/LazyBoolPRF.hpp>

namespace grafpe::domain::detail {

template <typename PRNG_T>
auto create_boolPRF(const vec_u8_t& seed, const tweak_t& tweak, uint64_t range)
-> RangeBoolPRF<PRNG_T> {
    typename PRNG_T::key_type key {};
    typename PRNG_T::iv_type   iv {};

    generate_key_iv<typename PRNG_T::ctx_type>(seed, tweak, key, iv);
    return RangeBoolPRF<PRNG_T>{PRNG_T{key, iv}, range};
}

template <typename PRNG_T, typename value_type>
auto create_boolPRF(const vec_u8_t& seed, const tweak_t& tweak)
-> LazyBoolPRF<PRNG_T, value_type> {
    typename PRNG_T::key_type key {};
    typename PRNG_T::iv_type   iv {};

    generate_key_iv<typename PRNG_T::ctx_type>(seed, tweak, key, iv);
    return LazyBoolPRF<PRNG_T, value_type>(key);
}

}  // namespace grafpe::domain::detail

