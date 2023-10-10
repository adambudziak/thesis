/*
 * author: Adam Budziak
 */

#pragma once

#include <algorithm>
#include <utility>

#include <openssl/evp.h>
#include <openssl/aes.h>

#include <grafpe/common/type_utils.hpp>

namespace grafpe::aes::openssl {

class AesCtr256Ctx {
    EVP_CIPHER_CTX *evp;

 public:
    static constexpr auto BLOCK_BYTES_LEN = 16;
    static constexpr auto   KEY_BYTES_LEN = 32;
    static constexpr auto    IV_BYTES_LEN = 16;
    static constexpr auto            NAME = "aes-256-ctr";

    using   key_type = std::array<uint8_t, KEY_BYTES_LEN>;
    using    iv_type = std::array<uint8_t, IV_BYTES_LEN>;
    using block_type = std::array<uint8_t, BLOCK_BYTES_LEN>;

    AesCtr256Ctx(const key_type& key, const iv_type& iv) : evp {EVP_CIPHER_CTX_new()} {
        EVP_EncryptInit(evp, EVP_aes_256_ctr(), key.data(), iv.data());
    }

    AesCtr256Ctx(uint8_t *key, uint8_t *iv): evp {EVP_CIPHER_CTX_new()} {
        EVP_EncryptInit(evp, EVP_aes_256_ctr(), key, iv);
    }

    ~AesCtr256Ctx() {
        EVP_CIPHER_CTX_free(evp);
    }

    // EVP_CIPHER_CTX probably non-copyable
    AesCtr256Ctx(AesCtr256Ctx&) = delete;
    AesCtr256Ctx& operator=(const AesCtr256Ctx&) = delete;

    AesCtr256Ctx(AesCtr256Ctx&& other) noexcept : evp {other.evp} {
        other.evp = nullptr;
    }

    AesCtr256Ctx& operator=(AesCtr256Ctx&& other) noexcept {
        swap(*this, other);
        return *this;
    }

    EVP_CIPHER_CTX* getEvp() const { return evp; }

    friend void swap(AesCtr256Ctx& lhs, AesCtr256Ctx& rhs) {
        using std::swap;
        swap(lhs.evp, rhs.evp);
    }
};

class AesCtr256 {
 public:
    using ctx_type = AesCtr256Ctx;
    using   key_type = ctx_type::key_type;
    using    iv_type = ctx_type::iv_type;
    using block_type = ctx_type::block_type;

    template<typename Container>
    static vec_u8_t encrypt(AesCtr256Ctx& ctx, const Container &msg);
    template<typename Container>
    static vec_u8_t decrypt(AesCtr256Ctx& ctx, const Container &crypt);

    template<typename Container>
    static void decrypt(AesCtr256Ctx& ctx, const Container &crypt, uint8_t *dest);

    template<typename Container>
    static void encrypt(AesCtr256Ctx& ctx, const Container &msg, uint8_t *dest);
};

template<typename Container>
vec_u8_t AesCtr256::encrypt(AesCtr256Ctx& ctx, const Container& msg) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    vec_u8_t msg_((msg.size()|15u)+1);
    std::copy(msg.begin(), msg.end(), msg_.begin());
    vec_u8_t cipher(msg_.size());
    encrypt(ctx, msg_, cipher.data());
    return cipher;
}

template<typename Container>
void AesCtr256::encrypt(AesCtr256Ctx &ctx, const Container &msg, uint8_t *dest) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    int size = 0, final_size = 0;

    EVP_EncryptUpdate(
            ctx.getEvp(), dest,
            &size, msg.data(),
            static_cast<int>(msg.size()));

    EVP_EncryptFinal(ctx.getEvp(), dest + size, &final_size);
}

template<typename Container>
vec_u8_t AesCtr256::decrypt(AesCtr256Ctx &ctx, const Container &crypt) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    vec_u8_t msg(crypt.size(), '\0');

    decrypt(ctx, crypt, msg.data());
    return msg;
}

template<typename Container>
void AesCtr256::decrypt(AesCtr256Ctx &ctx, const Container &crypt, uint8_t *dest) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    int size = 0, final_size = 0;

    EVP_DecryptUpdate(
            ctx.getEvp(),
            dest,
            &size,
            crypt.data(),
            static_cast<int>(crypt.size()));

    EVP_DecryptFinal(ctx.getEvp(), dest + size, &final_size);
}

}  // namespace grafpe::aes::openssl
