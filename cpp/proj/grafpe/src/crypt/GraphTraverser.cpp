/*
 * author: Adam Budziak
 */

#include <grafpe/crypt/GraphTraverser.hpp>
#include <iostream>
#include <grafpe/common/print_utils.hpp>
#include <bitset>

namespace grafpe {

using cell_t = perm_t::value_type;

cell_t GraphTraverser::one_step(cell_t prev, cell_t pos,
                                const vec_u8_t &buffer) const {
    auto[ti, dir] = get_permutation(buffer, cryptCtx->f-1, pos);

    return dir ? cryptCtx->inv_permutations[ti][prev]
           :  cryptCtx->permutations[ti][prev];
}

cell_t GraphTraverser::one_step_back(cell_t prev, cell_t pos,
                                     const vec_u8_t& buffer) const {
    auto[ti, dir] = get_permutation(buffer, cryptCtx->f-1, pos);
    return dir ? cryptCtx->permutations[ti][prev]
           : cryptCtx->inv_permutations[ti][prev];
}


cell_t GraphTraverser::go(cell_t start, const vec_u8_t& B) const {
    cell_t x = start;
    for (cell_t i = 0; i < cryptCtx->walk_length; ++i) {
        x = one_step(x, i, B);
    }
    return x;
}


cell_t GraphTraverser::go_back(cell_t start, const grafpe::vec_u8_t &B) const {
    cell_t x = start;
    for (cell_t i = cryptCtx->walk_length; i > 0; --i) {
        x = one_step_back(x, i - 1, B);
    }
    return x;
}

}  // namespace grafpe
