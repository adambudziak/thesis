/*
 * author: Adam Budziak
 */

#include <grafpe/graph/utils.hpp>

bool grafpe::is_graph_valid(const Graph &graph) {
    return !valid_permutations(graph.permutations).has_value();
}
