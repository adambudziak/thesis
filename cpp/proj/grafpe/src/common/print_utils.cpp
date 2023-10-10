/*
 * author: Adam Budziak
 */

#include <string>
#include <algorithm>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/common/print_utils.hpp>


namespace grafpe {



std::string bytes_to_hex(const uint8_t *msg, size_t len) {
    constexpr static char CHARS[] = "0123456789ABCDEF";
    std::string res(len*2, '\0');
    for (size_t i = 0; i < len; ++i) {
        res[i*2] = CHARS[msg[i] >> 4];
        res[i*2 + 1] = CHARS[msg[i] & 0xf];
    }
    return res;
}

vec_u8_t hex_to_bytes(std::string msg) {
    auto char_to_hex = [](char c) -> uint8_t {
        return static_cast<uint8_t>(c >= 'A' ? c - 'A' + 10 : c - '0');
    };

    if (msg.size() % 2 == 1) {
        msg.insert(msg.begin(), '\0');
    }

    vec_u8_t result(msg.size() / 2, '\0');

    for (size_t i = 0; i < msg.size(); i += 2) {
        result[i / 2] = (char_to_hex(msg[i]) << 4u) | (char_to_hex(msg[i+1]) & 0xf);
    }

    return result;
}

bool is_hex(const std::string& msg) {
    constexpr static char ALLOWED[] {"0123456789ABCDEFabcdef"};
    constexpr static auto not_allowed = [](char c) -> bool {
        return std::find(std::begin(ALLOWED), std::end(ALLOWED), c) == std::end(ALLOWED);
    };
    return std::find_if(msg.begin(), msg.end(), not_allowed) == msg.end();
}

}  // namespace grafpe
