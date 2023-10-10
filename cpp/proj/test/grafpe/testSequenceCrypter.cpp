/*
 * author: Adam Budziak
 */

#include <grafpe/crypt/Cast.hpp>
#include <grafpe/crypt/Grafpe.hpp>

#include "../catch/catch.hpp"

using namespace grafpe;
using namespace grafpe::crypter;

TEST_CASE("Basic test of sequence crypter") {

    class SequenceDerived: public CastCrypter<Grafpe<>, std::string> {
        using Crypter_T = Grafpe<>;
        using SequenceBase = CastCrypter<Grafpe<>, std::string>;
    public:
        explicit SequenceDerived(Crypter_T grafpe): SequenceBase(std::move(grafpe)) {}

    protected:
        grafpe::perm_t cast_impl(const std::string& msg) const override {
            return {msg.begin(), msg.end()};
        }

        std::string uncast_impl(const perm_t& perm) const override {
            return {perm.begin(), perm.end()};
        }
    };

    const auto key = array_from_string<Grafpe<>::key_type>("0000111122223333");
    const auto  iv = array_from_string<Grafpe<>::iv_type>("3333222211110000");

    std::string msg {"Hello, world!"};

    SequenceDerived seq_crypter { Grafpe<>{key, iv, 128, 4, 100} };
    REQUIRE(seq_crypter.decrypt(seq_crypter.encrypt(msg)) == msg);
}