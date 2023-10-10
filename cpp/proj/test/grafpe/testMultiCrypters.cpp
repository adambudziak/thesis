/*
 * author: Adam Budziak
 */

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/domain/CycleWalker.hpp>
#include <grafpe/domain/CycleSlicer.hpp>
#include <grafpe/domain/ReverseCycleWalker.hpp>
#include <grafpe/rte/Container.hpp>
#include <grafpe/crypt/Targeted.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>

#include <grafpe/config.hpp>

#include "../catch/catch.hpp"

using namespace grafpe;

TEST_CASE("Domain targeting with Containers crypter") {
    using prng_t = grafpe::aes::openssl::AesCtr128PRNG;

    using Grafpe = crypter::ScalarGrafpe<prng_t>;

    namespace tgt = grafpe::domain::targeting;
    namespace rte = grafpe::rte;

    auto key = hex_to_array<prng_t::key_type>("4C08CD92FCE6ACE874D24C2D91622ED7");
    auto  iv = hex_to_array<prng_t::iv_type>("16DF1433E5050459072C3DD89EAA8B82");

    auto names = std::vector<std::string> {
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

    using namespace std::string_literals;

    auto grafpe = Grafpe{key, iv, names.size(), 4, 100};

    SECTION("Cycle walker on the internal Grafpe") {
        using CWalker = tgt::CycleWalker<Grafpe>;
        auto c_walker = CWalker{[](const point_t& str) {
            return !(str % 5);
        }};
        auto targeted = crypter::Targeted{c_walker, grafpe};

        rte::Container crypter {
            names,
            targeted
        };

        REQUIRE(crypter.encrypt("Asia") == "Pita"s);
        REQUIRE(crypter.encrypt("Pita") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pioter") == "Asia"s);
    }

    SECTION("Cycle walker on the top") {
        using NamesCrypter = rte::Container<decltype(names), Grafpe>;
        using CWalker = tgt::CycleWalker<NamesCrypter>;

        auto names_crypter = rte::Container{names, grafpe};

        auto c_walker = CWalker{[](const std::string_view& str){
            return str[0] == 'P';
        }};

        auto crypter = crypter::Targeted{
            c_walker,
            names_crypter
        };

        REQUIRE(crypter.encrypt("Pita") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pioter") == "Ploter"s);
        REQUIRE(crypter.encrypt("Ploter") == "Pita"s);
        REQUIRE(crypter.encrypt("Porter") == "Porter"s);
    }

    SECTION("Cycle slicer on the internal Grafpe") {
        using CSlicer = tgt::CycleSlicer<Grafpe, prng_t>;

        auto c_slicer = CSlicer{{key, iv}, [](const point_t& a, const point_t& b) {
            return !(a % 5) && !(b % 5);
        }, 1000};

        auto targeted = crypter::Targeted{c_slicer, grafpe};

        rte::Container crypter {names, targeted};

#ifdef EXTENDED_CYCLE_SLICER
        REQUIRE(crypter.encrypt("Asia") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pita") == "Pita"s);
        REQUIRE(crypter.encrypt("Pioter") == "Asia"s);
#else
        REQUIRE(crypter.encrypt("Asia") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pita") == "Pita"s);
        REQUIRE(crypter.encrypt("Pioter") == "Asia"s);
#endif
    }

    SECTION("Reverse cycle walker on the internal Grafpe") {
        using RCWalker = tgt::ReverseCycleWalker<Grafpe, prng_t>;
        auto swp_data = hex_to_bytes("16DF1433E5050459072C3DD89EAA8B82");

        auto rc_walker = RCWalker{swp_data, [](const point_t& x) { return !(x % 5); }, 100};

        auto targeted = crypter::Targeted{rc_walker, grafpe};

        rte::Container crypter {names, std::move(targeted)};

#ifdef EXTENDED_RC_WALKER
        REQUIRE(crypter.encrypt("Asia") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pioter") == "Pita"s);
        REQUIRE(crypter.encrypt("Pita") == "Asia"s);
#else
        REQUIRE(crypter.encrypt("Asia") == "Pioter"s);
        REQUIRE(crypter.encrypt("Pioter") == "Asia"s);
        REQUIRE(crypter.encrypt("Pita") == "Pita"s);
#endif
    }

#ifdef EXTENDED_CYCLE_SLICER

    SECTION("Cycle slicer on the top") {

        using NamesCrypter = rte::Container<decltype(names), Grafpe>;
        using CSlicer = tgt::CycleSlicer<NamesCrypter>;

        auto names_crypter = rte::Container{names, grafpe};

        auto c_slicer = CSlicer{{key, iv},
                [](const std::string_view& a, const std::string_view& b){
            return a[0] == 'P' && b[0] == 'P';
        }, 10000};

        auto crypter = crypter::Targeted{
            c_slicer,
            names_crypter
        };

        REQUIRE(crypter.encrypt("Pita") == "Ploter"s);
        REQUIRE(crypter.encrypt("Pioter") == "Pita"s);
        REQUIRE(crypter.encrypt("Ploter") == "Porter"s);
        REQUIRE(crypter.encrypt("Porter") == "Pioter"s);
    }

#endif
    // RCW and Cycle Slicer don't support different formats than numerical
}
