/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <utility>

#include <dfare/dfa/RankedDFA.hpp>
#include "../crypt/Cast.hpp"

namespace grafpe::rte {

template <typename Crypter_T>
class DFA : public crypter::CastCrypter<Crypter_T, std::string> {
    using dfa_t = dfare::dfa::RankedDFA;
    dfa_t dfa;

 public:
    using value_type = std::string;
    using index_type = typename Crypter_T::value_type;

    DFA(dfa_t dfa, Crypter_T crypter):
        crypter::CastCrypter<Crypter_T, std::string>{std::move(crypter)}, dfa{std::move(dfa)} {
        static_assert(std::is_integral_v<index_type>);
    }

protected:
    index_type cast_impl(const value_type &value) const override;
    value_type uncast_impl(const index_type &value) const override;
};

template<typename Crypter_T>
auto DFA<Crypter_T>::cast_impl(const std::string& value) const -> index_type {
    return dfa.rank(value);
}

template<typename Crypter_T>
std::string DFA<Crypter_T>::uncast_impl(const index_type& value) const {
    return dfa.unrank(value);
}

template<typename Crypter_T>
DFA(dfare::dfa::RankedDFA, std::shared_ptr<Crypter_T>) -> DFA<Crypter_T>;


}  // namespace grafpe::rte
