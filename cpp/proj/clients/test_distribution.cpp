/*
 * author: Adam Budziak
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <pur/argparser/ArgParser.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/common/print_utils.hpp>

const auto HELP_STR = R"(
TestDistribution
n
d
)";

using namespace grafpe;

bool validate_args(const pur::argparser::defaultParser &parser) {
    if (!(
            parser.has_value("n")
            && parser.has_value("d")
            && parser.has_value("key")
            && parser.has_value("iv")
            && parser.has_value("walk_length"))) {
        std::cout << "not all parameters set" << std::endl;
        return false;
    }

    if (parser.get_value<int>("n") < 8) {
        std::cout << "n     must be greater than 7";
        return false;
    }

    if (parser.get_value<int>("d") < 2) {
        std::cout << "d     must be greater than 1";
        return false;
    }

    return true;
}

template <typename Crypter_T>
auto get_key_iv(const pur::argparser::ArgParser<>& parser) {
    try {
        auto key = hex_to_array<typename Crypter_T::key_type>(parser.get_value<std::string>("key"));
        auto  iv = hex_to_array<typename Crypter_T:: iv_type>(parser.get_value<std::string>("iv"));
        return std::pair{key, iv};
    } catch (const std::runtime_error& exc) {
        std::cerr << "Invalid length of key or iv\n";
        exit(1);
    }
}

int main(int argc, char **argv) {
    using Crypter = grafpe::crypter::Grafpe<grafpe::aes::openssl::AesCtr128PRNG>;

    pur::argparser::defaultParser parser{};
    parser.register_arg<int>("n");
    parser.register_arg<int>("d");
    parser.register_arg<std::string>("message");
    parser.register_arg<std::string>("input_file");
    parser.register_arg<std::string>("output_file");
    parser.register_arg<std::string>("key");
    parser.register_arg<std::string>("iv");
    parser.register_arg<int>("walk_length");

    try {
        parser.parse_program_params(argc, argv);
    } catch (const pur::argparser::InvalidArgumentExc& e) {
        std::cerr << e.what() << "\nAborting\n";
        exit(1);
    }

    if (!validate_args(parser)) {
        std::cout << HELP_STR << std::endl;
        exit(1);
    }

    auto N = static_cast<point_t>(parser.get_value<int>("n"));

    const auto[key, iv] = get_key_iv<Crypter>(parser);

    auto crypter = Crypter{
        key, iv,
        N,
        static_cast<std::size_t>(parser.get_value<int>("d")),
        static_cast<std::size_t>(parser.get_value<int>("walk_length"))
    };

    auto transform_plaintext = [N](auto &&txt) -> perm_t {
        perm_t target(txt.size());
        std::transform(txt.begin(), txt.end(), target.begin(),
                       [N](auto &&c) {return c % N; });
        return target;
    };

    perm_t plaintext;
    if (!parser.has_value("input_file")) {
        plaintext = transform_plaintext(parser.get_value<std::string>("message"));
    } else {
        std::ifstream ifs(parser.get_value<std::string>("input_file"));
        std::string tmp;
        std::getline(ifs, tmp);
        plaintext = transform_plaintext(tmp);
    }

    using pur::stream_operators::operator<<;

    auto crypt = crypter.encrypt(plaintext);
    if (!parser.has_value("output_file")) {
        std::cout << crypt << std::endl;
    } else {
        std::ofstream ofs(parser.get_value<std::string>("output_file"));
        ofs << parser.get_value<int>("n") << std::endl;
        ofs << crypt << std::endl;
    }
}
