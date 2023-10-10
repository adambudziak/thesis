/*
 * author: Adam Budziak
 */

#include <pur/containers/bitset_view.hpp>

namespace pur {

bool bitset_view::operator[](size_t index) const {
    static constexpr auto _one = static_cast<cell_t>(1);
    return static_cast<bool>(*(m_data + index / CELL_BIT) & (_one << index % CELL_BIT));
}

}  // namespace pur
