/*
 * author: Adam Budziak
 */

#include <climits>
#include <cstddef>
#include <cstdint>
#include <ostream>

#pragma once

namespace pur {

/**
 * A non-owning  alternative to the pur::bitset class.
 */
class bitset_view {
    using cell_t = uint8_t;
    static constexpr auto CELL_BIT = CHAR_BIT;

    const uint8_t * const m_data;
    const size_t  m_size;

 public:
    template<typename Container>
    explicit bitset_view(const Container& cont)
    : m_data { cont.data() },
      m_size { cont.size() * sizeof(typename Container::value_type) * CHAR_BIT} {
    }

    bool operator[](size_t index) const;
    const cell_t* data() const { return m_data; }
    size_t size() const { return m_size; }
};

}  // namespace pur
