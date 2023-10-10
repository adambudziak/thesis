/*
 * author: Adam Budziak
 */

#pragma once

#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include <dfare/regexp/RegExpBase.hpp>
#include <dfare/common/Alphabet.hpp>

namespace dfare::dfa {

class DFA {
 public:
    using states_t = std::vector<regexp::regexp_ptr>;
    using edges_t  = std::map<uint64_t, std::map<char, uint64_t>>;
    using accept_states_t = std::vector<uint64_t>;

    const states_t states;
    const edges_t edges;
    const accept_states_t accepting_states;

    const Alphabet alphabet;

    DFA(states_t states, edges_t edges, accept_states_t accepting, Alphabet alphabet):
            states{std::move(states)}, edges{std::move(edges)},
            accepting_states{std::move(accepting)}, alphabet{std::move(alphabet)} {  }

    static DFA from_regex(const regexp::regexp_ptr& regex);

    uint64_t run(const std::string& input) const;
    bool accepts(const std::string& input) const;
    bool has_transition(uint64_t from, char label) const;
    uint64_t get_transition(uint64_t from, char label) const;


    friend std::ostream& operator<<(std::ostream&, const DFA&);
};

std::ostream& operator<<(std::ostream& os, const DFA::states_t& states);

}  // namespace dfare::dfa
