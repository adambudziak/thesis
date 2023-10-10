/*
 * author: Adam Budziak
 */

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/crypt/Tuple.hpp>
#include <grafpe/rte/Container.hpp>
#include <grafpe/rte/DFA.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Basic test of tuple crypter") {
    using namespace grafpe;
    using namespace aes::openssl;

    const auto names = std::vector {
        "Asia",
        "Basia",
        "Kasia",
        "Elzbieta",
        "Konrad",
        "Pioter",
        "Bioder",
        "Ploter",
        "Froter",
        "Porter",
        "Pita",
        "Hummus",
        "Adam",
    };

    using Grafpe = crypter::ScalarGrafpe<AesCtr128PRNG>;
    using AgeCrypter = crypter::Grafpe<AesCtr128PRNG>;

    auto name_crypter_engine = Grafpe{
        hex_to_array<AesCtr128::key_type>("912C314E129436D3B23C506C424A32B6"),
        hex_to_array<AesCtr128::iv_type>("CE438C4A1E3E92B707821B9E0599F62A"),
        names.size(),
        4,
        100};

    auto regex_dfa = dfare::dfa::RankedDFA::from_string("0(0+1)*0", 10);

    auto regex_crypter_engine = Grafpe{
            hex_to_array<AesCtr128::key_type>("09CAA86FF48E33EB037D1AA8BBF4A6EE"),
            hex_to_array<AesCtr128::iv_type>("19DD37C0665252F84DCCEE00899534C2"),
            regex_dfa.max_rank,
            4,
            100};

    crypter::Tuple tuple_crypter {
        rte::Container{names, std::move(name_crypter_engine)},
        AgeCrypter{
                hex_to_array<AesCtr128::key_type>("3EDB6F1706C55A835338C57E9161BF4B"),
                hex_to_array<AesCtr128::iv_type>("19ACB9397671AA6767C90AA0C750B163"),
                128,
                4,
                100
        },
        rte::DFA{
                regex_dfa, std::move(regex_crypter_engine)
        }
    };

    using namespace std::string_literals;

    auto result = tuple_crypter.encrypt("Kasia", perm_t{33}, "0110001000");
    REQUIRE(decltype(result){"Basia", perm_t{95}, "0001100000"} == result);
}
