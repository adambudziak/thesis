/*
 * author: Adam Budziak
 */

#pragma once

#include <vector>
#include <cstdint>
#include <array>

namespace grafpe {

template<uint64_t Size>
using arr_u8_t = std::array<uint8_t, Size>;

using vec_u8_t   = std::vector<uint8_t>;
using vec_u32_t  = std::vector<uint32_t>;
using vec_u64_t  = std::vector<uint64_t>;
using perm_t     = vec_u64_t;
using point_t    = typename perm_t::value_type;
using perm_vec_t = std::vector<perm_t>;

/**
 *
 * A helper class working as a common interface for a uint64_t and std::array<uint8_t, 8>.
 * It allows easier construction of tweaks in situations when a number is more natural
 * than array and vice versa.
 *
 */
class tweak_t {
    using tw_array_t = std::array<uint8_t, 8>;
    uint64_t m_data = 0;

    uint64_t get_data(const tw_array_t& data) {
        uint64_t result = data[7];
        for (int i = 6; i >= 0; --i)
            result = (result << 8) | data[i];
        return result;
    }

 public:
    tweak_t() = default;
    explicit tweak_t(uint64_t num): m_data{num} {}
    explicit tweak_t(const tw_array_t & data) : m_data{get_data(data)} {}

    const uint64_t* data() const {
        return &m_data;
    }
};

}  // namespace grafpe
