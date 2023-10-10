/*
 * author: Adam Budziak
 */

#include <algorithm>
#include <iostream>
#include <string>

#include <dfare/dfa/RankedDFA.hpp>

namespace dfare::dfa {

RankedDFA::table_t RankedDFA::build_table(size_t word_length) const {
    table_t table;

    for (auto& state : dfa.accepting_states) {
        table[{state, 0}] = 1;
    }

    for (size_t i = 0; i < word_length; ++i) {
        for (size_t state = 0; state < dfa.states.size(); ++state) {
            for (auto& c : dfa.alphabet.characters()) {
                table[{state, i+1}] += table[{dfa.get_transition(state, c), i}];
            }
        }
    }

    return table;
}

std::size_t RankedDFA::rank(const std::string &regex) const {
    std::size_t q = 0;
    std::size_t c = 0;
    for (size_t i = 0; i < regex.size(); ++i) {
        for (auto chr = dfa.alphabet.cbegin();
                  chr < std::find(dfa.alphabet.cbegin(), dfa.alphabet.cend(), regex[i]); ++chr) {
            c += rank_table[{dfa.get_transition(q, *chr), n - i - 1}];
        }
        q = dfa.get_transition(q, regex[i]);
    }
    return c;
}

std::string RankedDFA::unrank(std::size_t rank) const {
    std::string regex;
    int q = 0;
    int j = 0;

    for (std::size_t i = 1; i <= n; ++i) {
        auto chr = dfa.alphabet.chr(j);

        auto val = rank_table[{dfa.get_transition(q, chr), n - i}];
        while (rank >= val) {
            rank -= val;
            ++j;
            chr = dfa.alphabet.chr(j);
            val = rank_table[{dfa.get_transition(q, chr), n - i}];
        }
        regex += chr;
        q = dfa.get_transition(q, chr);
        j = 0;
    }
    return regex;
}

size_t RankedDFA::compute_max_rank() const {
    return rank_table[{0, n}];
}

bool RankedDFA::accepts(const std::string &input) const noexcept {
    return dfa.accepts(input);
}

}  // namespace dfare::dfa
