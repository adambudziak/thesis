/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/common/type_utils.hpp>

namespace grafpe {

class Graph {
 public:
    uint64_t n;
    uint64_t d;
    perm_vec_t permutations;
};

}  // namespace grafpe
