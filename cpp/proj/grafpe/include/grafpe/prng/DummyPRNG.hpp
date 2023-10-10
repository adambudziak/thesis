/*
 * author: Adam Budziak
 */

#pragma once

#include <random>
#include <functional>
#include <array>
#include <limits>

#include <grafpe/common/type_utils.hpp>
#include "PRNG.hpp"

namespace grafpe {

class DummyCtx {
 public:
    using block_type = std::array<uint8_t, 16>;
};


class DummyPRNG: public PRNG_<DummyPRNG, DummyCtx> {
    friend PRNG_<DummyPRNG, DummyCtx>;
    using rand_engine_t = std::mt19937;
    using distribution_t = std::uniform_int_distribution<rand_engine_t::result_type >;

    static constexpr size_t MAX_VAL = std::numeric_limits<uint8_t>::max();

    rand_engine_t generator;
    distribution_t distribution;

 public:
    using   ctx_type = DummyCtx;
    using block_type = ctx_type::block_type;

    DummyPRNG() {
        generator.seed(std::random_device {}());
        distribution = distribution_t(0, MAX_VAL);
    }

    void fill_block(uint8_t *data) {
        for (int i = 0; i < 16; ++i) {
            *(data + i) = static_cast<uint8_t>(distribution(generator));
        }
    }

    template<typename Container>
    void fill_bytes(Container& buffer) {
        static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
        for (auto &i : buffer) {
            i = static_cast<uint8_t >(distribution(generator));
        }
    }

    void fill_block(DummyCtx::block_type &block) {
        for (auto &i : block) {
            i = static_cast<uint8_t>(distribution(generator));
        }
    }
};

}  // namespace grafpe
