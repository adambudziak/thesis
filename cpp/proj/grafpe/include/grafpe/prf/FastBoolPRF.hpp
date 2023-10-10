/*
 * author: Adam Budziak
 */

#pragma once

#include <iostream>
#include <grafpe/common/print_utils.hpp>

#include <algorithm>
#include <utility>
#include <pur/containers/bitset_view.hpp>
#include "../common/type_utils.hpp"

#ifdef HAS_BOOST
#include <boost/container_hash/hash.cpp>
#endif

namespace grafpe {

template <typename BlockCipher_T,
          typename value_type>
class FastBoolPRF {
 public:
    using key_type = typename BlockCipher_T::key_type;
    using  iv_type = typename BlockCipher_T::iv_type;

private:
    using ctx_type = typename BlockCipher_T::ctx_type;
    key_type key;
    iv_type iv;

 public:
    explicit FastBoolPRF(key_type key, iv_type iv)
    : key {std::move(key)}, iv {std::move(iv)} {}

    bool operator()(const value_type& value, const tweak_t& tweak) {
        vec_u8_t digest(ctx_type::BLOCK_BYTES_LEN, 0);
        std::size_t hash = 0;
        if constexpr (std::is_same_v<value_type, std::size_t>) {
            hash = value;
        } else {
#ifdef HAS_BOOST
            hash = boost::hash_value(value);
#else
            hash = std::hash<value_type>{}(value);
#endif
        }
        hash += *tweak.data();

        std::copy_n(reinterpret_cast<uint8_t*>(&hash), sizeof(hash), digest.data());
        auto ctx = ctx_type{key, iv};
        auto cipher = BlockCipher_T::encrypt(ctx, digest);

        return pur::bitset_view {cipher}[0];
    };
};

}
