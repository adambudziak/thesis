/*
 * author: Adam Budziak
 */

#pragma once

#include <cmath>
#include <string>
#include <algorithm>
#include "type_utils.hpp"

namespace grafpe {

template<typename T>
T vec_from_string(const std::string &str) {
    return T{str.begin(), str.end()};
}

/**
 * Parses a c-string into an std::array of bytes, where
 * each character of the c-string represents a single element in the output.
 *
 * It requires the c-string to have an appropriate number of characters,
 * if this condition is not met, the compilation will fail.
 * @tparam Arr - array type to cast the c-string to. It is required that the Arr
 *               type has a constexpr size function.
 *               In all situations, the size of Arr has to be equal to the length
 *               of the c-string. The zero-termination sign is omitted.
 * @tparam N - length of the c-string to cast
 * @param str - c-string to cast
 * @return an Arr object containing casted c-string.
 */
template<typename Arr, int N>
constexpr Arr array_from_string(const char (&str)[N]) {
    Arr arr {};
    static_assert(arr.size() == N-1);
    std::copy_n(std::begin(str), arr.size(), arr.begin());
    return arr;
}

template<typename Arr>
Arr array_from_string(const std::string &str) {
    Arr arr {};
    std::copy_n(str.begin(), std::min(arr.size(), str.size()), arr.begin());
    return arr;
}

std::string bytes_to_hex(const uint8_t* msg, size_t len);

template<typename Container>
std::string bytes_to_hex(const Container &bytes) {
    return bytes_to_hex(static_cast<const uint8_t*>(bytes.data()),
                        bytes.size() * sizeof(typename Container::value_type));
}

bool is_hex(const std::string& msg);

vec_u8_t hex_to_bytes(std::string msg);

template<typename Array>
constexpr Array hex_to_array(const std::string_view& msg) {
    if (std::tuple_size<Array>::value * 2 != msg.size()) {
        throw std::runtime_error("hex_to_array array <-> hex size mismatch");
    }
    constexpr auto char_to_hex = [](char c) -> uint8_t {
        return static_cast<uint8_t>(c >= 'A' ? c - 'A' + 10 : c - '0');
    };

    Array array {};

    for (size_t i = 0; i < msg.size(); i += 2) {
        array[i / 2] = (char_to_hex(msg[i]) << 4u) | (char_to_hex(msg[i+1]) & 0xf);
    }

    return array;
}

}  // namespace grafpe
