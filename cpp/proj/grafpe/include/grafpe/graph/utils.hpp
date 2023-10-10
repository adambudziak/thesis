/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/graph/Graph.hpp>
#include <grafpe/permutation/utils.hpp>

namespace grafpe {

/**
 * Checks if the graph conforms the requirements:
 *  - no loops, multiple edges or cycles of length 2
 *  - every edge has equal degree (= d) ~ graph is regular
 *
 * @param graph - graph to be checked
 * @return - boolean indicating if the graph is valid
 */
bool is_graph_valid(const Graph& graph);
}  // namespace grafpe
