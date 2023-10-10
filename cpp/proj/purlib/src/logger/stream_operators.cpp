/*
 * author: Adam Budziak
 */

#include <string>
#include <pur/logger/stream_operators.hpp>

namespace pur::stream_operators::quote_operators {

std::ostream& operator<<(std::ostream& os, char c) {
    // TODO(abudziak) super lame >:(  refactor
    std::string cp {"\"x\""};
    cp[1] = c;
    os << cp;
    return os;
}
}  // namespace pur::stream_operators::quote_operators
