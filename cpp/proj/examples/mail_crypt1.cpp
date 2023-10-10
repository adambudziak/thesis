/*
 * author: Adam Budziak
 */

#include <iostream>
#include <cassert>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/common/print_utils.hpp>
#include <dfare/dfa/RankedDFA.hpp>
#include <grafpe/rte/DFA.hpp>

using namespace grafpe;
using namespace std::string_literals;
using grafpe::aes::openssl::AesCtr128PRNG;

int main() {
    // This script shows the first way to implement encryption of email addresses.
    // This approach is the simplest one, where the only crypter used is
    // the regex-crypter itself. It is also the worst solution.

    // There are however a few drawbacks:
    //  - the language described by a regular expression grows drastically
    //    in the length of the expression, so it may require too much memory
    //    to encrypt it using GraFPE
    //  - the results may be biased. Since there are 26-times as many 4-character
    //    nick-names than 3-character nick-names, it is much more possible that
    //    a 4-character nick-name will be selected. Since the length of the
    //    domain is restricted, this limits the chance than one of the longer
    //    site domain (such as gmail/onet) will be selected.

    // First: prepare random key and iv
    auto key = random_array<AesCtr128PRNG::key_type>(DummyPRNG{});
    auto  iv = random_array<AesCtr128PRNG::iv_type>(DummyPRNG{});

    std::cout << "Key and IV: "
              << bytes_to_hex(key) << '\n'
              << bytes_to_hex(iv) << '\n';


    // first we need to create a ranked DFA from the regex
    const auto plaintext {"b@onet.com"s};
    const std::string email_regex = "[a-z][a-z]*@(wp + onet).(com + pl + eu + net)";

    auto dfa = dfare::dfa::RankedDFA::from_string(email_regex, plaintext.size());

    std::cerr << "The maximal rank of the regex: " << dfa.max_rank << std::endl;

    try {
        // may fail if the regex is too long
        auto grafpe = crypter::ScalarGrafpe<AesCtr128PRNG>{
                key, iv,
                dfa.max_rank,             // number of vertices == size of the domain
                4,                        // degree of the graph. bigger == slower preparation
                1000};                    // length of random walks. bigger == safer == slower

        auto regex_crypter = rte::DFA { dfa, grafpe };

        std::cout << regex_crypter.encrypt(plaintext) << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "The domain of the DFA is too big for grafpe allocation. Aborting\n";
        std::exit(1);
    }

}
