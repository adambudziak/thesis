/*
 * author: Adam Budziak
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <limits>
#include <climits>
#include <cstdint>
#include <vector>

namespace pur {

class bitset {
    using   cell_t = uint64_t;
    using buffer_t = std::vector<cell_t>;

    static constexpr auto bit_size = sizeof(cell_t) * 8;

    buffer_t m_data;
    const size_t m_size;

 public:
    using value_type = bool;


    template<typename Container>
    bitset(const Container& container, size_t size);

    bool operator[](size_t index) const;

    const cell_t* data() const { return m_data.data(); }

    size_t size() const { return m_size; }

    size_t bytes_cnt() const { return (size() - 1) / CHAR_BIT + 1; }
};


template<typename Container>
bitset::bitset(const Container &container, size_t size)
    : m_size { size }
{
    using value_t = typename Container::value_type;
    static_assert(std::numeric_limits<value_t>::is_integer);

    const auto requested_bytes_cnt = (size - 1) / CHAR_BIT + 1;
    const auto buffer_bytes_cnt = container.size() * sizeof(value_t);
    const auto bytes_to_copy = std::min(requested_bytes_cnt, buffer_bytes_cnt);

    m_data.resize(requested_bytes_cnt);
    std::memcpy(m_data.data(), container.data(), bytes_to_copy);
}

}  // namespace pur
