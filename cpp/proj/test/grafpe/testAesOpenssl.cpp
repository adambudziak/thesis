/*
 * author: Adam Budziak
 */

#include <cstring>
#include <string>

#include <grafpe/common/type_utils.hpp>
#include <grafpe/aes/openssl/AesCtr128.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>

#include "../catch/catch.hpp"


using namespace grafpe;
using namespace grafpe::aes;

TEST_CASE("Basic encryption and decryption") {
    using cipher_t = openssl::AesCtr128;

    const auto key = array_from_string<cipher_t::key_type>("0000111122223333");
    const auto iv = array_from_string<cipher_t::iv_type>("3333222211110000");

    cipher_t::ctx_type ctx{key, iv};

    auto is_prefix = [](const std::string& str, const vec_u8_t& ciph) -> bool {
        return std::string {ciph.begin(), ciph.begin() + str.size()} == str;
    };

    auto is_decrypted = [&is_prefix](const std::string& str, const vec_u8_t& ciph) -> bool {
        return is_prefix(str, ciph)
               && std::string {ciph.begin() + str.size(), ciph.end()}
                    == std::string(ciph.size() - str.size(), '\0');
    };

    SECTION("Decrypt(Encrypt(M)) = M") {
        WHEN("Message is short") {
            std::string plain{"Hello world"};

            auto cipher = cipher_t::encrypt(ctx, vec_from_string<vec_u8_t>(plain));


            THEN("After decryption we get the message back again (possibly padded)") {
                cipher_t::ctx_type dctx{key, iv};
                auto decr = cipher_t::decrypt(dctx, cipher);

                REQUIRE(is_decrypted(plain, decr));
            }
        }

        WHEN("Message is long") {
            std::string plain{"some message longer than 16 chars"};

            auto cipher = cipher_t::encrypt(ctx, vec_from_string<vec_u8_t>(plain));

            THEN("After decryption we get the message back again (possibly padded)") {
                cipher_t::ctx_type dctx{key, iv};

                auto decr = cipher_t::decrypt(dctx, cipher);

                REQUIRE(is_decrypted(plain, decr));
            }
        }
    }
}

TEST_CASE("AesCtr128 pseudo-random number generator") {
    using cipher_t = openssl::AesCtr128;

    const auto key = array_from_string<cipher_t::key_type>("0000111122223333");
    const auto iv = array_from_string<cipher_t::iv_type>("3333222211110000");

    WHEN("Seeding with predefined key and iv") {
        openssl::AesCtr128PRNG prng{key, iv};

        vec_u8_t buffer(24, '\0');
        THEN("The result is deterministic") {
            prng.fill_bytes(buffer);
            REQUIRE(grafpe::bytes_to_hex(buffer)
                    == "D0496FD1D6A280B7CCDDB8FE4D7068D2922C7A6A0A10417F");
        }
    }
}

TEST_CASE("AesCtr128PRNG creates different outputs") {
    using cipher_t = grafpe::aes::openssl::AesCtr128;
    std::array<vec_u8_t, 100> buffers;

    const auto key = array_from_string<cipher_t::key_type>("0000111122223333");
    const auto iv = array_from_string<cipher_t::iv_type>("3333222211110000");

    openssl::AesCtr128PRNG prng{key, iv};

    for (auto& buffer : buffers) {
        buffer.resize(32);
        prng.fill_bytes(buffer);
    }

    auto it = std::unique(buffers.begin(), buffers.end());
    REQUIRE(it == buffers.end());
}
