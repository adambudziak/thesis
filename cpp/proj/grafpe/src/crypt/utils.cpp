/*
 * author: Adam Budziak
 */

#include <utility>
#include <grafpe/crypt/utils.hpp>
#include <grafpe/permutation/utils.hpp>

namespace grafpe {


bool get_bit(const vec_u8_t &buffer, size_t i) {
    return static_cast<bool>(buffer[i / 8] & (0x80u >> (i % 8)));
}


uint32_t get_bits(uint32_t source, uint32_t start, uint32_t end) {
    if (!start) { return source >> static_cast<uint32_t>(32 - end); }
    return (source >> static_cast<uint32_t>(32 - end)) & ((1u << static_cast<uint32_t>(end - start)) - 1);
}



uint32_t get_bits(const vec_u8_t &buffer, uint32_t start, uint32_t end) {
    uint32_t result = buffer[start / 8];
    uint32_t byte_width = (end-1)/8 - start / 8 + 1;
    uint32_t i = 1;
    for (; i < byte_width; ++i) {
        result = (result << 8u) | buffer[start / 8 + i];
    }
    result <<= (4 - i) * 8;
    return get_bits(result, start % 8, (start % 8) + end - start);
}


bool get_direction(const vec_u8_t &buffer, int bit_width, size_t i) {
    return get_bit(buffer, (i+1)*(bit_width + 1) - 1);
}


uint32_t get_permutation_index(const vec_u8_t &buffer, int bit_width, size_t i) {
    auto first = static_cast<uint32_t>(i * (bit_width + 1));

    uint32_t res = 0;
    for (uint32_t index = 0; index < static_cast<uint32_t>(bit_width); ++index) {
        res |= get_bit(buffer, first + index) << (bit_width - index - 1);
    }
    return res;
}


vec_u8_t get_digest(const perm_t& xs, const perm_t& ys, size_t index) {
    const auto tsize = (xs.size()-1) * sizeof(perm_t::value_type);
    const auto tindex = index * sizeof(perm_t::value_type);
    if (tsize == 0) { return {0}; }

    vec_u8_t result(tsize);
    std::memcpy(result.data(), ys.data(), tindex);
    std::memcpy(result.data() + tindex, xs.data()+index+1, tsize-tindex);

    return result;
}


std::pair<uint32_t, bool> get_permutation(const vec_u8_t &buffer, int bits_cnt, size_t i) {
    auto bits = get_bits(buffer, i * bits_cnt, i * bits_cnt + bits_cnt);
    return {bits/2, bits%2};
}

}  // namespace grafpe
