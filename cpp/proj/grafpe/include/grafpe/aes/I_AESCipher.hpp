/*
 * author: Adam Budziak
 */

#pragma once

#include <openssl/evp.h>
#include <grafpe/common/type_utils.hpp>


namespace grafpe {
namespace aes {

class I_AESCtx {
 public:
    virtual EVP_CIPHER_CTX* getEvp() const = 0;
};

template<typename Derived>
class I_AESCipher {
 public:
    /**
     * High level encryption method that takes care of the padding and stuff.
     * It is guaranteed to work for all possible input
     * @param ctx
     * @param msg
     * @return
     */
    static vec_u8_t encrypt(I_AESCtx& ctx, const vec_u8_t& msg) {
        return Derived::encrypt_(ctx, msg);
    }

    /**
     * High level decryption method
     *
     * @param ctx
     * @param msg
     * @return
     */
    static vec_u8_t decrypt(I_AESCtx& ctx, const vec_u8_t& crypt) {
        return Derived::decrypt_(ctx, crypt);
    }


    /**
     * Low-level encryption method that needs the parameters to be checked
     * outside.
     *
     * @param ctx
     * @param msg  - needs to have size in multiples of 16
     * @param dest - needs to have at least the same size as msg
     */
    static void encrypt(I_AESCtx& ctx, const vec_u8_t & msg, uint8_t* dest) {
        Derived::encrypt_(ctx, msg, dest);
    }


    /**
     * Low-level decryption method.
     * Analogous to encrypt(ctx, msg, dest).
     * @param ctx
     * @param crypt
     * @param dest
     */
    static void decrypt(I_AESCtx& ctx, const vec_u8_t& crypt, uint8_t* dest) {
        Derived::decrypt_(ctx, crypt, dest);
    }

    virtual ~I_AESCipher() = default;

 private:
    I_AESCipher() = default;
    friend Derived;
};

}  // namespace aes
}  // namespace grafpe
