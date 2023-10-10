/*
 * author: Adam Budziak
 */

#pragma once

#include <map>
#include <string>
#include <utility>

#include <dfare/regexp/from_string.hpp>
#include <pur/optional/Optional.hpp>
#include "DFA.hpp"

namespace dfare::dfa {

class RankedDFA {
    using table_t = std::map<std::pair<uint64_t, uint64_t>, uint64_t>;

 public:
    const DFA dfa;
    const size_t n;

 private:
    mutable table_t rank_table;

 public:
    const size_t max_rank;
    explicit RankedDFA(DFA dfa, size_t word_length):
            dfa {std::move(dfa)},
            n{word_length},
            rank_table{build_table(word_length)},
            max_rank {compute_max_rank()} {
    }

    static RankedDFA from_string(const std::string& str, std::size_t word_length) {
        return RankedDFA {DFA::from_regex(regexp::from_string(str)),  word_length};
    }

    [[nodiscard]] bool accepts(const std::string& input) const noexcept;

    [[nodiscard]] const table_t& get_rank_table() const noexcept { return rank_table; }

    std::size_t rank(const std::string& regex) const;
    std::string unrank(std::size_t rank) const;

 private:
    table_t build_table(std::size_t word_length) const;
    size_t compute_max_rank() const;
};

}  // namespace dfare::dfa
