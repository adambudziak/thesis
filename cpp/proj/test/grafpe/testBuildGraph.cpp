/*
 * author: Adam Budziak
 */

#include "../catch/catch.hpp"

#include <grafpe/graph/utils.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>



TEST_CASE("Graph builder builds graphs") {
    using namespace grafpe;
    using prng_t = aes::openssl::AesCtr128PRNG;

    const auto key = array_from_string<prng_t::key_type>("0000111122223333");
    const auto  iv = array_from_string<prng_t::iv_type>("3333222211110000");

    const int N = 10000, D = 4;

    auto builder = GraphBuilder<prng_t>::create_from_engine({key, iv});

    auto graph = builder.build(N, D);
    REQUIRE(graph.n == N);
    REQUIRE(graph.d == D);
    REQUIRE(is_graph_valid(graph));
}


TEST_CASE("GraphBuilder factory works") {
    using namespace grafpe;
    using prng_t = aes::openssl::AesCtr128PRNG;

    const auto key = array_from_string<prng_t::key_type>("0000111122223333");
    const auto  iv = array_from_string<prng_t::iv_type>("3333222211110000");

    const int N = 10000, D = 4;
    auto builder = GraphBuilder<prng_t>::create_from_engine({key, iv});
    auto graph = builder.build(N, D);
    REQUIRE(graph.n == N);
    REQUIRE(graph.d == D);
    REQUIRE(is_graph_valid(graph));
}
