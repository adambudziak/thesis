/*
 * author: Adam Budziak
 */

#include <string>
#include <fstream>

#include <pur/argparser/ArgParser.hpp>
#include <dfare/dfa/DFA.hpp>
#include <dfare/regexp/from_string.hpp>
#include "../grafpe/include/grafpe/rte/DFA.hpp"

int main(int argc, char **argv) {
    using namespace dfare::dfa;
    pur::argparser::defaultParser parser{};
    parser.register_arg<std::string>("regexp");
    parser.register_arg<std::string>("output_file");

    parser.parse_program_params(argc, argv);

    if (!parser.has_value("regexp")) {
        std::cerr << "regexp missing\n";
        exit(1);
    }
    std::ofstream file_output;
    std::ostream* os = &std::cout;

    auto regex_str = parser.get_value<std::string>("regexp");
    if (parser.has_value("output_file")) {
        file_output.open(parser.get_value<std::string>("output_file"));
    }
    if (!file_output.is_open()) {
        std::cerr << "Couldn't open the given file" << std::endl;
        std::cerr << "Falling back to stdout" << std::endl;
    } else {
        os = &file_output;
    }

    auto regex = dfare::regexp::from_string(regex_str);

    auto dfa = DFA::from_regex(regex);
    std::cout << dfa.alphabet.characters() << std::endl;

    using pur::stream_operators::operator<<;
    *os << dfa.states << '\n'
        << dfa.edges << '\n'
        << dfa.accepting_states << std::endl;

    auto ranked_dfa = RankedDFA::from_string(regex_str, 5);

    *os << ranked_dfa.get_rank_table() << '\n';

}
