/*
 * author: Adam Budziak
 */

#include <valarray>
#include <string>
#include <unordered_set>

#include "../catch/catch.hpp"
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>


TEST_CASE("Grafpe is deterministic") {
    using namespace grafpe;
    using namespace aes::openssl;
    using Crypter = crypter::Grafpe<AesCtr128PRNG>;

    auto key = array_from_string<Crypter::key_type>("0123012301233210");

    auto  iv = array_from_string<Crypter::iv_type>("3210321032103210");

    point_t N = 256;
    std::size_t D = 4, walk_length = 4;

    auto crypter = Crypter{key, iv, N, D, walk_length};
    auto crypt = crypter.encrypt(vec_from_string<perm_t>("Hello world!"));

    REQUIRE(bytes_to_hex(vec_u8_t{crypt.begin(), crypt.end()}) == std::string{"4D2C09144BAB4ED7406D67C9"});
}


TEST_CASE("Decrypt(Encrypt(M)) == M") {
    using namespace grafpe;
    using namespace aes;
    using namespace aes::openssl;
    using Crypter = crypter::Grafpe<AesCtr128PRNG>;

    vec_u8_t key_ = hex_to_bytes("000011112222333344445555666677778888");
    vec_u8_t  iv_ = hex_to_bytes("888877776666555544443333222211110000");

    Crypter::key_type key;
    Crypter::iv_type   iv;

    std::copy_n(key_.begin(), key.size(), key.begin());
    std::copy_n(iv_.begin(), iv.size(), iv.begin());

    const point_t N = 10000;
    const std::size_t D = 4;

    const auto plaintexts = {
            "",
            "a",
            "dog",
            "hello",
            "short_msg",
            "longer message",
            "pretty long nice message",
            "hello from the other siiiiii iiide",
            "The grass was greener, The light was brighter, The taste was sweeter, The night of wonder"
    };

    for (size_t walk_length = 1; walk_length < 30; walk_length += 3) {
        auto crypter = Crypter{key, iv, N, D, 100};
        for (auto& plaintext : plaintexts) {
            auto perm_plain = vec_from_string<perm_t>(plaintext);
            auto crypt = crypter.encrypt(perm_plain);
            auto msg = crypter.decrypt(crypt);
            REQUIRE(perm_plain == msg);
        }
    }
}


TEST_CASE("Crypters only depend on the key and iv") {
    using namespace grafpe;
    using namespace aes;
    using namespace aes::openssl;

    using grafpe::aes::openssl::AesCtr128Ctx;

    auto key_ = hex_to_bytes("000011112222333344445555666677778888");
    auto  iv_ = hex_to_bytes("888877776666555544443333222211110000");

    AesCtr128Ctx::key_type key;
    AesCtr128Ctx::iv_type   iv;

    std::copy_n(key_.begin(), key.size(), key.begin());
    std::copy_n(iv_.begin(), iv.size(), iv.begin());

    const point_t N = 10000;
    const std::size_t D = 4;

    auto builder_alice = GraphBuilder<AesCtr128PRNG>::create_from_engine({key, iv});
    crypter::Grafpe<> alice{builder_alice.build(N, D), 4, key};

    auto builder_bob = GraphBuilder<AesCtr128PRNG>::create_from_engine({key, iv});
    crypter::Grafpe<> bob{builder_bob.build(N, D), 4, key};

    std::string secret {"But call me maybe"};
    auto u32_secret = vec_from_string<perm_t>(secret);

    REQUIRE(u32_secret == bob.decrypt(alice.encrypt(u32_secret)));
    REQUIRE(u32_secret == alice.decrypt(bob.encrypt(u32_secret)));
}

TEST_CASE("Graph configurations have to be the same") {
    using namespace grafpe;
    using namespace grafpe::aes;
    using Crypter = crypter::Grafpe<openssl::AesCtr128PRNG>;

    const auto key = array_from_string<Crypter::key_type>("0000111122223333");
    const auto  iv = array_from_string<Crypter::iv_type>("3333222211110000");

    const point_t N = 256;
    std::size_t D = 4, walk_length = 4;

    auto alice = Crypter{key, iv, N, D, walk_length};
    auto   bob = Crypter{key, iv, N, D, walk_length};

    auto plaintext = vec_from_string<perm_t>("hello world");

    REQUIRE(alice.decrypt(bob.encrypt(plaintext)) == plaintext);

    SECTION("Keys and ivs") {
        WHEN("Key is different") {
            auto key2 = key;
            key2[0] = 1;
            THEN("Message cannot be decrypted") {
                auto eve = Crypter{key2, iv, N, D, walk_length};
                REQUIRE(eve.decrypt(alice.encrypt(plaintext)) != plaintext);
            }
        }

        WHEN("IV is different") {
            auto iv2 = iv;
            iv2[0] = 1;
            THEN("Message cannot be decrypted") {
                auto eve = Crypter{key, iv2, N, D, walk_length};
                REQUIRE(eve.decrypt(alice.encrypt(plaintext)) != plaintext);
            }
        }

        WHEN("N is different") {
            const point_t N1 = N + 1;
            THEN("Message cannot be decrypted") {
                auto eve = Crypter{key, iv, N1, D, walk_length};
                REQUIRE(eve.decrypt(alice.encrypt(plaintext)) != plaintext);
            }
        }

        WHEN("D is different") {
            const std::size_t D1 = D - 2;
            THEN("Message cannot be decrypted") {
                auto eve = Crypter{key, iv, N, D1, walk_length};
                REQUIRE(eve.decrypt(alice.encrypt(plaintext)) != plaintext);
            }
        }

        WHEN("walk_length is different") {
            const std::size_t walk_length1 = walk_length + 3;
            THEN("Message cannot be decrypted") {
                auto eve = Crypter{key, iv, N, D, walk_length1};
                REQUIRE(eve.decrypt(alice.encrypt(plaintext)) != plaintext);
            }
        }
    }
}

TEST_CASE("Different plaintexts have different ciphers") {
    using namespace grafpe;
    using namespace grafpe::aes;
    using Crypter = crypter::Grafpe<openssl::AesCtr128PRNG>;

    const auto key = array_from_string<Crypter::key_type>("0000111122223333");
    const auto  iv = array_from_string<Crypter::iv_type>("3333222211110000");

    const point_t N = 256;
    const std::size_t D = 4, walk_length = 4;

    auto alice = Crypter{key, iv, N, D, walk_length};

    SECTION("Very short messages") {
        perm_t ciphers(256);
        for (uint32_t i = 0; i < 256; ++i) {
            ciphers[i] = alice.encrypt(perm_t{i})[0];
        }
        REQUIRE(is_permutation(ciphers));
    }
}

TEST_CASE("Encrypting numbers") {
    using namespace grafpe;
    using namespace grafpe::aes;
    using ScCrypter = crypter::ScalarGrafpe<openssl::AesCtr128PRNG>;
    using Crypter = crypter::Grafpe<openssl::AesCtr128PRNG>;

    const auto key = array_from_string<Crypter::key_type>("0000111122223333");
    const auto  iv = array_from_string<Crypter::iv_type>("3333222211110000");

    const point_t N = 256;
    const std::size_t D = 4, walk_length = 4;

    auto alice = Crypter{key, iv, N, D, walk_length};
    auto   bob = ScCrypter{key, iv, N, D, walk_length};

    SECTION("1-element vector operations are equivalent to number operations") {
        REQUIRE(alice.encrypt(perm_t {77})[0] == bob.encrypt(77));

        REQUIRE(bob.decrypt(alice.encrypt(perm_t{99})[0]) == 99);
        REQUIRE(bob.decrypt(alice.encrypt(perm_t{103})[0]) == 103);

        REQUIRE(alice.decrypt({bob.encrypt(103)}) == perm_t{103});
    }
}

TEST_CASE("Grafpe with tweaks") {
    using namespace grafpe;
    using namespace grafpe::aes;
    using Crypter = crypter::Grafpe<openssl::AesCtr128PRNG>;

    const auto key = array_from_string<Crypter::key_type>("aaaabbbbccccdddd");
    const auto  iv = array_from_string<Crypter::iv_type>("ddddccccbbbbaaaa");

    const point_t N = 256;
    const std::size_t D = 4, walk_length = 64;

    auto alice = Crypter{key, iv, N, D, walk_length};

    auto dec_enc = [&alice](const perm_t& plaintext, uint32_t t1, uint32_t t2) {
        return alice.decrypt(alice.encrypt(plaintext, tweak_t{t1}), tweak_t{t2});
    };


    auto plaintext = vec_from_string<perm_t>("hello");

    SECTION("Tweaks must be the same for encryption and decryption") {
        WHEN("Tweaks are the same") {
            REQUIRE(dec_enc(plaintext, 10, 10) == plaintext);
            REQUIRE(dec_enc(plaintext, 33, 33) == plaintext);
            REQUIRE(dec_enc(plaintext, 0, 0) == plaintext);
        }

        WHEN("Tweaks are different") {
            REQUIRE(dec_enc(plaintext, 0, 1) != plaintext);
            REQUIRE(dec_enc(plaintext, 100, 5) != plaintext);
            REQUIRE(dec_enc(plaintext, 5, 123) != plaintext);
        }
    }

    SECTION("Tweak == 0 is equivalent to no tweak") {
        REQUIRE(alice.decrypt(alice.encrypt(plaintext), tweak_t{0}) == plaintext);
        REQUIRE(alice.decrypt(alice.encrypt(plaintext, tweak_t{0})) == plaintext);
    }
}
