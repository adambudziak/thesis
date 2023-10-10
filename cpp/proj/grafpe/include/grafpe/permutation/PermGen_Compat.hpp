/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <numeric>
#include <algorithm>

#include <pur/logger/Logger.hpp>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/permutation/utils.hpp>

namespace grafpe {
namespace permutation {

/**
 * Class generating pseudo-random permutations compatible with algorithm
 * described in the protocol (Inverse Riffle Shuffle).
 *
 * It's slower than the custom Generator but it's correct.
 *
 * @tparam PRNG_T - a class than can serve as a PRNG
 */
template<typename PRNG_T>
class Generator_Compat {
    mutable PRNG_T prng;

 public:
    using prng_engine_t     = PRNG_T;
    explicit Generator_Compat(PRNG_T &&prng) : prng{std::move(prng)} {  }

    perm_t create_permutation(size_t size) const;
    void repermute(perm_t &nums, int check_index = -1) const;

    void initialize() {
        prng.initialize();
    }

 private:
    struct gen_ctx_t {
        perm_t copied_nums;
        vec_u64_t bit_slots;
        vec_u64_t copied_slots;

        static gen_ctx_t create(size_t size) {
            return {
                perm_t(size), vec_u64_t(size), vec_u64_t(size)
            };
        }
    };

    void fast_permute(perm_t& nums, gen_ctx_t &ctx) const;
    void repermute(perm_t& nums, gen_ctx_t ctx) const;
    bool single_permute(perm_t& nums, gen_ctx_t &ctx) const;
};

template<typename PRNG_T>
perm_t Generator_Compat<PRNG_T>::create_permutation(size_t size) const {
    perm_t nums(size);
    std::iota(nums.begin(), nums.end(), 0);
    auto ctx = gen_ctx_t::create(size);

    while (!single_permute(nums, ctx)) {}

    auto bad = valid_permutation(nums);
    while (bad.has_value()) {
        fast_permute(nums, ctx);
        bad = valid_permutation(nums);
    }
    return nums;
}

template<typename PRNG_T>
bool Generator_Compat<PRNG_T>::single_permute(perm_t& nums, gen_ctx_t &ctx) const {
    fast_permute(nums, ctx);
    return std::adjacent_find(ctx.bit_slots.begin(), ctx.bit_slots.end()) == ctx.bit_slots.end();
}

template<typename PRNG_T>
void Generator_Compat<PRNG_T>::fast_permute(perm_t &nums, gen_ctx_t &ctx) const {
    int odd_count = 0;
    for (auto& slot : ctx.bit_slots) {
        slot = (slot << 1) | prng.fetch_bit();
        odd_count += slot & 1;
    }

    int odd_moved = 0, even_moved = 0;

    const auto size = static_cast<int>(ctx.bit_slots.size());
    int position;
    for (int i = size - 1; i >= 0; --i) {
        auto current = ctx.bit_slots[i];
        if (current % 2) {
            position = size - odd_moved - 1;
            ++odd_moved;
        } else {
            position = size - odd_count - even_moved - 1;
            ++even_moved;
        }
        ctx.copied_slots[position] = current;
        ctx.copied_nums[position] = nums[i];
    }

    nums.swap(ctx.copied_nums);
    ctx.bit_slots.swap(ctx.copied_slots);
}

template<typename PRNG_T>
void Generator_Compat<PRNG_T>::repermute(perm_t &nums, int) const {
    repermute(nums, gen_ctx_t::create(nums.size()));
}

template<typename PRNG_T>
void Generator_Compat<PRNG_T>::repermute(perm_t &nums, gen_ctx_t ctx) const {
    decltype(valid_permutation(perm_t{})) bad;
    do {
        fast_permute(nums, ctx);
        bad = valid_permutation(nums);
    } while (bad.has_value());
}


}  // namespace permutation
}  // namespace grafpe
