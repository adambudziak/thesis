/*
 * author: Adam Budziak
 */

#pragma once

#include <memory>
#include <cstdint>
#include <utility>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/crypt/utils.hpp>

namespace grafpe {

/**
 *
 * Class implementing the walk on the graph.
 *
 *
 */
class GraphTraverser {
    using cell_t = perm_t::value_type;
    std::shared_ptr<const CryptCtx> cryptCtx;

 public:
    GraphTraverser(Graph &&graph, size_t walk_length)
            : cryptCtx { std::make_shared<CryptCtx>(std::move(graph), walk_length)} {
    }

    cell_t go(cell_t start, const vec_u8_t& B) const;
    cell_t go_back(cell_t start, const vec_u8_t& B) const;

    const CryptCtx& getContext() const {
        return *cryptCtx;
    }

 private:
    cell_t      one_step(cell_t prev, cell_t pos, const vec_u8_t& buffer) const;
    cell_t one_step_back(cell_t prev, cell_t pos, const vec_u8_t& buffer) const;
};

}  // namespace grafpe
