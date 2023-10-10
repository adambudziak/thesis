/*
 * author: Adam Budziak
 */

#include <cstring>
#include <limits>
#include <pur/containers/bitset.hpp>

namespace pur {


bool bitset::operator[](size_t index) const {
    return static_cast<bool>(m_data[index / bit_size] & (cell_t(1) << index % bit_size));
}

}
