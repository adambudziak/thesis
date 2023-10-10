/*
 * author: Adam Budziak
 */

#pragma once

#include <cstddef>
#include <utility>

#include <pur/logger/Logger.hpp>
#include <grafpe/permutation/PermGen_Compat.hpp>
#include "../prng/PRNG.hpp"
#include <grafpe/permutation/utils.hpp>
#include "../permutation/PermGen.hpp"
#include "Graph.hpp"

namespace grafpe {

struct BuilderPolicy {};

struct Compat : BuilderPolicy{};
struct Prune : BuilderPolicy{};

namespace detail {

template <typename Policy>
struct Builder_Impl {

    template <typename Generator>
    static Graph build(Generator& perm_gen, uint64_t n, uint64_t d);
};

template <>
template <typename Generator>
Graph Builder_Impl<Compat>::build(Generator& perm_gen, uint64_t n, uint64_t d) {
    perm_vec_t permutations {};
    permutations.reserve(d / 2);
    for (uint64_t i = 0; i < d / 2; ++i) {

#ifdef GRAFPE_VERBOSE
        std::cerr << "Generating permutation number: " << (i + 1) << '\n';
#endif
        permutations.emplace_back(perm_gen.create_permutation(n));
        auto bad_vertex = valid_last_permutation(permutations);

        const auto last = std::prev(permutations.end());
        while (bad_vertex.has_value()) {
            perm_gen.repermute(*last, bad_vertex.value());
            bad_vertex = valid_last_permutation(permutations);
        }
    }
    return Graph{
            n,
            d,
            permutations
    };
}

template <>
template <typename Generator>
Graph Builder_Impl<Prune>::build(Generator& perm_gen, uint64_t n, uint64_t d) {
    perm_vec_t permutations {};
    permutations.resize(d / 2);
    bool all_valid = false;
    while (!all_valid) {
        for (auto i = 0; i < d / 2; ++i) {
            permutations[i] = perm_gen.create_permutation(n);
        }

        auto valid = valid_permutations(permutations);

#ifdef GRAFPE_VERBOSE
        if (valid.has_value()) {
            std::cerr << valid.value() << " permutations valid\n";
        } else {
            std::cerr << "All permutations valid!\n";
        }
#endif
        all_valid = !valid.has_value();
    }

    return Graph{
            n,
            d,
            permutations
    };
}

}  // namespace detail


/**
 * Class responsible for constructing the graph given a permutation generator and configuration.
 *
 * @tparam Generator - a permutation generator class having following methods:
 *      * repermute
 *      * create_permutation    - to create a pseudo-random permutation on [n]
 */
template<typename prng_engine_t, typename Generator = permutation::Generator_Compat<prng_engine_t>>
class GraphBuilder {
    Generator perm_gen;

 public:
    explicit GraphBuilder(Generator perm_gen)
      : perm_gen{std::forward<Generator>(perm_gen)}
    { }

    template <typename Policy=Compat>
    Graph build(uint64_t n, uint64_t d) const {
        return detail::Builder_Impl<Policy>::build(perm_gen, n, d);
    }

    void initialize() {
        perm_gen.initialize();
    }

    static GraphBuilder<prng_engine_t, Generator> create_from_engine(prng_engine_t&& engine) {
        return GraphBuilder<prng_engine_t, Generator>(Generator{std::forward<prng_engine_t>(engine)});
    }
};

}  // namespace grafpe
