/*
 * author: Adam Budziak
 */

#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <functional>

#include <pur/logger/stream_operators.hpp>
#include <dfare/dfa/DFA.hpp>
#include <dfare/regexp/Eps.hpp>
#include <dfare/regexp/from_string.hpp>
#include <dfare/regexp/RegExpBase.hpp>

namespace dfare::dfa {

struct recursively_build {
    DFA::states_t& states;
    DFA::edges_t& edges;
    const Alphabet& alphabet;

    auto find_equivalent(const regexp::regexp_ptr& deriv) {
        auto equiv = [&deriv](const auto& other) { return deriv.equivalent(other); };
        return std::find_if(states.begin(), states.end(), equiv);
    }

    void operator()(const regexp::regexp_ptr& regex) {
        auto current_index = states.size()-1;

        for (char c : alphabet.characters()) {
            auto deriv = regex.derivative(c);
            if (auto equiv = find_equivalent(deriv); equiv != states.end()) {
                edges[current_index][c] = static_cast<std::size_t>(equiv - states.begin());
            } else {
                states.push_back(deriv);
                edges[current_index][c] = states.size() - 1;
                (*this)(deriv);
            }
        }
    }
};


DFA DFA::from_regex(const regexp::regexp_ptr& regex) {
    std::ostringstream oss;
    oss << regex;
    auto alphabet = regexp::get_alphabet(oss.str());
    DFA::states_t states{regex};
    DFA::edges_t edges {};
    DFA::accept_states_t accepting {};

    recursively_build {states, edges, alphabet}(regex);

    for (uint64_t index = 0; index < states.size(); ++index) {
        if (states[index].matches_empty()) accepting.push_back(index);
    }

    states.push_back(regexp::Empty::create());
    return {states, edges, accepting, alphabet};
}


std::ostream& operator<<(std::ostream& os, const DFA& dfa) {
    using pur::stream_operators::operator<<;
    using regexp::operator<<;
    os << "Q: " << dfa.states << '\n'
       << "d: " << dfa.edges << '\n'
       << "F: " << dfa.accepting_states;
    return os;
}


uint64_t DFA::run(const std::string &input) const {
    uint64_t q = 0;
    for (char c : input) {
        if (edges.count(q) == 0 || edges.at(q).count(c) == 0) {
            return states.size() - 1;
        }
        q = edges.at(q).at(c);
    }
    return q;
}


bool DFA::accepts(const std::string &input) const {
    auto result_state = run(input);
    return std::find(
            accepting_states.begin(),
            accepting_states.end(), result_state) != accepting_states.end();
}


bool DFA::has_transition(uint64_t from, char label) const {
    return edges.count(from) && edges.at(from).count(label);
}


uint64_t DFA::get_transition(uint64_t from, char label) const {
    return has_transition(from, label) ? edges.at(from).at(label) : states.size()-1;
}

}  // namespace dfare::dfa
