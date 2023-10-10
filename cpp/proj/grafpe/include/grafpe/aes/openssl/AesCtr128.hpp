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


class AesCtr128Ctx {
    EVP_CIPHER_CTX *evp;

 public:
    static constexpr auto BLOCK_BYTES_LEN = 16;
    static constexpr auto KEY_BYTES_LEN = 16;
    static constexpr auto  IV_BYTES_LEN = 16;
    static constexpr auto          NAME = "aes-128-ctr";

    using   key_type = std::array<uint8_t, KEY_BYTES_LEN>;
    using    iv_type = std::array<uint8_t, IV_BYTES_LEN>;
    using block_type = std::array<uint8_t, BLOCK_BYTES_LEN>;

    AesCtr128Ctx(const key_type& key, const  iv_type& iv) : evp {EVP_CIPHER_CTX_new()} {
        EVP_EncryptInit(evp, EVP_aes_128_ctr(), key.data(), iv.data());
    }

    AesCtr128Ctx(uint8_t *key, uint8_t *iv) : evp {EVP_CIPHER_CTX_new()} {
        EVP_EncryptInit(evp, EVP_aes_128_ctr(), key, iv);
    }

    ~AesCtr128Ctx() {
        EVP_CIPHER_CTX_free(evp);
    }

    // EVP_CIPHER_CTX is non-copyable? Not sure.
    AesCtr128Ctx(AesCtr128Ctx&) = delete;
    AesCtr128Ctx& operator=(const AesCtr128Ctx&) = delete;

    AesCtr128Ctx(AesCtr128Ctx&& other) noexcept: evp{other.evp} {
        other.evp = nullptr;
    }

    AesCtr128Ctx& operator=(AesCtr128Ctx&& other) noexcept {
        swap(*this, other);
        return *this;
    };

    EVP_CIPHER_CTX* getEvp() const { return evp; }

    friend void swap(AesCtr128Ctx& lhs, AesCtr128Ctx& rhs) {
        using std::swap;
        swap(lhs.evp, rhs.evp);
    }
};


class AesCtr128 {
 public:
    using   ctx_type = AesCtr128Ctx;
    using   key_type = ctx_type::key_type;
    using    iv_type = ctx_type::iv_type;
    using block_type = ctx_type::block_type;

    template<typename Msg_T>
    static vec_u8_t encrypt(AesCtr128Ctx& ctx, const Msg_T &msg);

    template<typename Msg_T>
    static vec_u8_t decrypt(AesCtr128Ctx& ctx, const Msg_T &crypt);

    template<typename Msg_T>
    static void decrypt(AesCtr128Ctx &ctx, const Msg_T &crypt, uint8_t *dest);

    template<typename Msg_T>
    static void encrypt(AesCtr128Ctx &ctx, const Msg_T &msg, uint8_t *dest);
};

template<typename Container>
vec_u8_t openssl::AesCtr128::encrypt(AesCtr128Ctx& ctx, const Container& msg) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    if (msg.size() % 16) {
        vec_u8_t msg_((msg.size()|15u)+1);      // pad to the smallest multiple of 16B
        std::copy(msg.begin(), msg.end(), msg_.begin());
        vec_u8_t cipher(msg_.size());
        encrypt(ctx, msg_, cipher.data());
        return cipher;
    } else {
        vec_u8_t cipher(msg.size());
        encrypt(ctx, msg, cipher.data());
        return cipher;
    }
}

template<typename Container>
vec_u8_t openssl::AesCtr128::decrypt(AesCtr128Ctx& ctx, const Container &crypt) {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    vec_u8_t msg(crypt.size(), '\0');

    decrypt(ctx, crypt, msg.data());
    return msg;
}

template<typename Container>
void openssl::AesCtr128::encrypt(AesCtr128Ctx &ctx, const Container &msg, uint8_t *dest) {
    static_assert(std::is_same_v<typename Container::value_type , uint8_t>);
    int size = 0, final_size = 0;

    EVP_EncryptUpdate(
            ctx.getEvp(),
            dest,
            &size,
            msg.data(),
            static_cast<int>(msg.size()));

    EVP_EncryptFinal(ctx.getEvp(), dest + size, &final_size);
}

template<typename Container>
void openssl::AesCtr128::decrypt(AesCtr128Ctx& ctx, const Container &crypt, uint8_t *dest) {
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
