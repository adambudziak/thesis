/*
 * author: Adam Budziak
 */

#define GRAFPE_VERBOSE

#include <iostream>
#include <chrono>
#include <string>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/aes/openssl/AesCtr256PRNG.hpp>
#include <grafpe/common/print_utils.hpp>
#include <pur/logger/stream_operators.hpp>
#include <fstream>

int main() {
    using namespace grafpe;
    using engine_t = grafpe::aes::openssl::AesCtr128PRNG;

//    const auto key = array_from_string<engine_t::key_type>("0000111122223333");
//    const auto  iv = array_from_string<engine_t::iv_type>("3333222211110000");

    const auto key = hex_to_array<engine_t::key_type>("01230123012301230123012301230123");
    const auto  iv = hex_to_array<engine_t::iv_type>("00001111222233330000111122223333");

    int N = 1000000, D = 8;
    auto builder = GraphBuilder<engine_t>::create_from_engine({key, iv});
    builder.initialize();

    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    auto graph = builder.build(N, D);
    auto end = high_resolution_clock::now();

    auto dur_ms = duration_cast<milliseconds>(end - start);
    std::cout << "Elapsed: " << dur_ms.count() << "ms"  << std::endl;

    std::ofstream file;
    file.open("graph_cpp.test");

    for (auto& p: graph.permutations) {
        file << "{\n";
        for (auto& i: p) {
            file << "\t\t" << i << '\n';
        }
        file << "}, ";
    }

}
