/*
 * author: Adam Budziak
 */

#pragma once

#include "../common/type_utils.hpp"
#include <unordered_set>
#include <pur/optional/Optional.hpp>

namespace grafpe {

/**
 * Function that checks if elements of a container create a permutation on the set
 * {1, 2, ..., size(Container)}
 *
 * @tparam Container - container class containing
 */
template<typename Container>
bool is_permutation(const Container &perm) {
    std::unordered_set<typename Container::value_type> unique;
    for (size_t i = 0; i < perm.size(); ++i) {
        if (perm[i] >= perm.size()) {
            return false;
        }
        if (auto[iter, ok] = unique.insert(perm[i]); !ok) {
            return false;
        }
    }
    return unique.size() == perm.size();
}

/**
 * Checks if the container represents a valid permutation.
 * A valid permutation is a permutation that doesn't have
 * loops or cycles of length 2.
 *
 * This function doesn't check if the data is generally
 * a permutation.
 *
 * @tparam Container - type of the container. It should have
 *          operator[] implemented and an efficient size function.
 * @param cont - container holding a permutation to verify.
 * @return  pur::Optional containing value of the first erroneous
 *          index. If no such index exists, the Optional is empty.
 */
template<typename Container>
pur::Optional<uint64_t> valid_permutation(const Container& cont) {
    const auto size = cont.size();
    for (size_t i = 0; i < size; ++i) {
        if (i == cont[cont[i]]) { return i; }
    }
    return {};
}


/**
 * Checks if the last permutation conforms the requirements for the
 * rest of the set of permutations.
 * @param permutations
 * @return int: i = {
 *  -1 if all is ok
 *  invalid index if one is found
 * }
 */
template<typename Container>
pur::Optional<uint64_t> valid_last_permutation(
        typename Container::const_iterator begin,
        typename Container::const_iterator end
        ) {
    const auto& last_permutation = *std::prev(end);
    const size_t last_size = last_permutation.size();

    for (size_t i = 0; i < last_size; ++i) {
        for (auto iter = begin; iter < end - 1; ++iter) {
            if (i == (*iter)[last_permutation[i]] || last_permutation[i] == (*iter)[i]) {
                return i;
            }
        }
    }
    return {};
}

template<typename Container>
pur::Optional<uint64_t> valid_last_permutation(const Container& cont) {
    return valid_last_permutation<Container>(cont.begin(), cont.end());
}


template<typename Container>
pur::Optional<uint64_t> valid_permutations(const Container& permutations) {
    auto begin = permutations.begin();
    for (auto last = std::next(begin); last < permutations.end(); ++last) {
        if (auto index = valid_last_permutation<Container>(begin, last); index.has_value()) {
            return index;
        }
    }
    return {};
}

template<typename Container>
Container inverse_permutation(const Container &permutation) {
    Container result(permutation.size());
    for (size_t i = 0; i < permutation.size(); ++i) {
        result[permutation[i]] = i;
    }
    return result;
}

perm_vec_t inverse_permutations(const perm_vec_t& perms);

}  // namespace grafpe
