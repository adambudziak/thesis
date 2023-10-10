/*
 * author: Adam Budziak
 */

#pragma once

#include <vector>
#include <utility>

#include <grafpe/common/type_utils.hpp>
#include <pur/containers/bitset_view.hpp>

namespace grafpe {

/**
 * Class representing an interface for any pseudo random function
 * with binary values (i.e. only 0 or 1).
 *
 * It's domain is a range {0, ..., N - 1}, where N is given during construction.
 *
 * It works by sampling the prng for every element of the range
 * and storing its output in an array. Then it allows to fetch
 * the bit for every value of the range.
 */
template<typename prng_engine_t>
class RangeBoolPRF {
 public:
    RangeBoolPRF(prng_engine_t&& prng, size_t range)
      : m_data{build_fn(std::move(prng), range)},
        m_data_bitset {m_data} { }

    bool operator()(size_t index) const {
        return m_data_bitset[index];
    }

    size_t size() const {
        return m_data.size();
    }

 private:
    vec_u8_t m_data;
    pur::bitset_view m_data_bitset;

    vec_u8_t build_fn(prng_engine_t&& prng, size_t range) {
        vec_u8_t fn((range - 1)/8 + 1);
        prng.fill_bytes(fn);
        return fn;
    }
};

}  // namespace grafpe
