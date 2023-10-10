/*
 * author: Adam Budziak
 */

#pragma once
#include <grafpe/config.hpp>

#include <utility>
#include <unordered_map>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/crypt/utils.hpp>
#ifdef HAS_BOOST
#include <boost/container_hash/hash.hpp>
#endif

namespace grafpe {

/**
 *
 * @tparam PRNG_T
 * @tparam value_type
 */
template <typename PRNG_T,
          typename value_type>
class LazyBoolPRF {
    using key_type = typename PRNG_T::key_type;
    using  iv_type = typename PRNG_T::iv_type;
 public:
    using seed_type = typename PRNG_T::key_type;

 private:
    seed_type m_seed;
    std::unordered_map<std::size_t, bool> m_data;

 public:
    explicit LazyBoolPRF(const seed_type& seed) : m_seed {seed} {  }

    bool operator()(const value_type& value) {
        std::size_t hash;
        if constexpr (std::is_same_v<value_type, std::size_t>) {
            hash = value;
        } else {
#ifdef HAS_BOOST
            hash = boost::hash_value(value);
#else
            hash = std::hash<value_type>{}(value);
#endif
        }
        if (auto coin = m_data.find(hash); coin != m_data.end()) {
            return coin->second;
        }
        key_type key {};
        iv_type   iv {};
        generate_key_iv<typename PRNG_T::ctx_type>(m_seed, tweak_t{hash}, key, iv);
        m_data[hash] = PRNG_T{key, iv}.fetch_bit();
        return m_data.at(hash);
    }
};

}
