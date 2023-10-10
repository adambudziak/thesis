/*
 * author: Adam Budziak
 */

#include <grafpe/permutation/utils.hpp>
#include <algorithm>
#include <cstdint>
#include <unordered_set>

namespace grafpe {

perm_vec_t inverse_permutations(const perm_vec_t &perms) {
    perm_vec_t result(perms.size());
    std::transform(perms.begin(), perms.end(), result.begin(), inverse_permutation<perm_t>);
    return result;
}


}

