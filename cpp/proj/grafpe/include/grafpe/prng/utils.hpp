/*
 * author: Adam Budziak
 */

#include <array>

#pragma once

namespace grafpe {

template<typename Array, typename prng_t>
Array random_array(prng_t&& prng) {
    using block_t = typename prng_t::block_type;
    Array array {};

    if constexpr (std::tuple_size<Array>::value == std::tuple_size<block_t>::value) {
        prng.fill_block(array);
    } else {
        prng.fill_bytes(array);
    }
    return array;
}

}  // namespace grafpe
