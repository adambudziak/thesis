/*
 * author: Adam Budziak
 */

#define GRAFPE_VERBOSE

#include <pur/argparser/ArgParser.hpp>

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/graph/utils.hpp>
#include <grafpe/common/print_utils.hpp>
#include <fstream>

void print_results(const grafpe::perm_vec_t& permutations,
                   std::optional<std::string> output_file) {

    std::ostream* target;
    std::ofstream file_stream;
    if (output_file.has_value()) {
        file_stream = std::ofstream{output_file.value()};
        target = &file_stream;
    } else {
        target = &std::cout;
    }

    *target << "graph {";
    for (std::size_t i = 0; i < permutations[0].size(); ++i) {
        *target << "\n\t" << i << ';';
    }
    for (const auto& perm: permutations) {
        for (std::size_t i = 0; i < permutations[0].size(); ++i) {
            *target << "\n\t" << i << " -- " << perm[i] << ';';
        }
    }
    *target << "\n}\n";
}

int main(int argc, char **argv) {
    using namespace grafpe;

    auto parser = pur::argparser::defaultParser{};

    parser.register_arg<int>("n");
    parser.register_arg<int>("d");
    parser.register_arg<std::string>("output-file");

    parser.parse_program_params(argc, argv);

    uint64_t N, D;

    if (!parser.has_value("n") || !parser.has_value(("d"))) {
        std::cerr << "Usage --n=<n> --d=<d> [--output-file=output-file]\n";
        exit(1);
    } else {
        N = static_cast<uint64_t>(parser.get_value<int>("n"));
        D = static_cast<uint64_t>(parser.get_value<int>("d"));
    }

    std::optional<std::string> output_file;
    if (parser.has_value("output-file")) {
        output_file = parser.get_value<std::string>("output-file");
    }

    using prng = aes::openssl::AesCtr128PRNG;

    auto key = random_array<prng::key_type>(DummyPRNG{});
    auto  iv = random_array<prng::iv_type>(DummyPRNG{});

    std::cerr << "Key and iv used: "
            << '\t' << bytes_to_hex(key) << '\n'
            << '\t' << bytes_to_hex(iv) << '\n';

    std::cerr << "N = " << N << "; D = " << D << '\n';

    auto builder = GraphBuilder<prng>::create_from_engine({key, iv});

    auto graph = builder.build<Prune>(N, D);

    std::cerr << "Generated " << graph.permutations.size() << " permutations\n";

    std::cerr << "Validating... " << (is_graph_valid(graph) ? "OK" : "FAILED") << '\n';

    print_results(graph.permutations, output_file);
}