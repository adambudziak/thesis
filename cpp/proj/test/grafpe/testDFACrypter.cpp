/*
 * author: Adam Budziak
 */

#include <string>

#include <grafpe/common/print_utils.hpp>
#include <grafpe/rte/DFA.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Basic tests of encryption/decryption of DFA") {
    using namespace grafpe;
    using Crypter = crypter::ScalarGrafpe<aes::openssl::AesCtr128PRNG>;

    const auto key = array_from_string<Crypter::key_type>("aaaabbbbccccdddd");
    const auto  iv = array_from_string<Crypter::iv_type>("ddddccccbbbbaaaa");

    SECTION("dec(enc(M)) == M") {
        auto ranked = dfare::dfa::RankedDFA::from_string("(a+b+c)*", 5);
        auto crypter_engine = Crypter{key, iv, ranked.max_rank, 4, 4};
        auto crypter = rte::DFA{ranked, std::move(crypter_engine)};


        auto inputs = {
                "abcab",
                "aaaaa",
                "ccccc",
                "abaca",
                "bbbbb",
                "abbac"
        };

        for (auto& input : inputs) {
            REQUIRE(crypter.decrypt(crypter.encrypt(input)) == input);
        }
    }
    SECTION("Encryption gives expected outputs") {
        auto regexp = "Nie____+Rusz___+Andziu_+tego___+kwiatka+roza___+kole___";

        auto dfa = dfare::dfa::RankedDFA::from_string(regexp, 7);
        auto crypter_engine = Crypter{key, iv, dfa.max_rank, 4, 4};
        auto crypter = rte::DFA{dfa, std::move(crypter_engine)};

        std::string results[] = {
                "Andziu_",
                "Nie____",
                "Rusz___",
                "kole___",
                "kwiatka",
                "roza___",
                "tego___"
        };

        for (int i = 0; i < dfa.max_rank; ++i) {
            REQUIRE(crypter.encrypt(dfa.unrank(i)) == results[i]);
        }
    }
}
