/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <vector>
#include <cstdint>
#include <numeric>

#include <grafpe/permutation/PermGen.hpp>
#include <grafpe/permutation/utils.hpp>
#include <grafpe/prng/PRNG.hpp>
#include <grafpe/common/type_utils.hpp>

namespace grafpe {
namespace permutation {

/**
 * Class generating pseudo-random permutations in a custom (faster) way than
 * described in the protocol (i.e. it's not Inverse Riffle Shuffle).
 *
 * The main difference is that it's not stable and possibly not secure.
 * But it's faster and creates random-looking permutations so it may
 * have some uses.
 *
 * @tparam PRNG_T class that can work as a PRNG
 *
 */
template<typename PRNG_T>
class Generator {
    mutable PRNG_T prng;

 public:
    using prng_engine_t     = PRNG_T;
    explicit Generator(PRNG_T &&prng)
            : prng{std::move(prng)} {  }

    perm_t create_permutation(size_t size) const;
    void repermute(perm_t &nums, int check_index = -1) const;

    void initialize() {
        prng.initialize();
    }

 private:
    bool single_permute(perm_t &nums, perm_t &bit_slots) const;
    bool fast_permute(perm_t &nums, const pur::Optional<uint64_t>&) const;
};

template<typename PRNG_T>
bool Generator<PRNG_T>::single_permute(perm_t &nums, perm_t &bit_slots) const {
    const auto size = static_cast<int>(nums.size());
    int last_swapped = 0;
    bool valid = true;
    auto previous = static_cast<perm_t::value_type>(-1);

    perm_t::value_type* current;
    for (int index = 0; index < size; ++index) {
        current = &bit_slots[index];
        perm_t::value_type one = prng.fetch_bit();
        *current = (*current << 1u) | one;

        if (one) {
            std::swap(nums[index], nums[last_swapped]);
            std::swap(*current, bit_slots[last_swapped]);
            current = &bit_slots[last_swapped++];
        }

        valid = valid && previous != *current;
        previous = *current;
    }
    return valid;
}

template<typename PRNG_T>
perm_t Generator<PRNG_T>::create_permutation(size_t size) const {
    perm_t nums(size);
    std::iota(nums.begin(), nums.end(), 0);
    perm_t bit_slots(size);

    while (!single_permute(nums, bit_slots)) {}

    auto bad = valid_permutation(nums);
    while (bad.has_value()) {
        while (!fast_permute(nums, bad)) {}
        bad = valid_permutation(nums);
    }
    return nums;
}


template<typename PRNG_T>
bool Generator<PRNG_T>::fast_permute(perm_t &nums, const pur::Optional<uint64_t>& check_index) const {
    const uint64_t size = nums.size();
    bool res = !check_index.has_value();
    uint64_t last_swapped = 0;

    for (uint64_t index = 0; index < size; ++index) {
        if (prng.fetch_bit()) {
            std::swap(nums[index], nums[last_swapped]);
            if (check_index == index || last_swapped++ == check_index) {
                return true;
            }
        }
    }
    return res;
}


template<typename PRNG_T>
void Generator<PRNG_T>::repermute(perm_t &nums, int check_index) const {
    while (!fast_permute(nums, check_index)) {}
    auto bad = valid_permutation(nums);
    while (bad.has_value()) {
        while (!fast_permute(nums, bad)) {}
        bad = valid_permutation(nums);
    }
}

}  // namespace permutation
}  // namespace grafpe
