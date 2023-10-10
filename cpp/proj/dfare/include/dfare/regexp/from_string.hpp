/*
 * author: Adam Budziak
 */

#pragma once

#include <string>

#include "RegExpBase.hpp"
#include <dfare/common/Alphabet.hpp>


namespace dfare::regexp {


regexp_ptr from_string(const std::string& str);
dfare::Alphabet get_alphabet(const std::string& str);


}
