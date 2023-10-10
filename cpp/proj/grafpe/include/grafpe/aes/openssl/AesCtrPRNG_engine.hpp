/*
 * author: Adam Budziak
 */

#pragma once

#include <algorithm>
#include "AesCtr128.hpp"

namespace grafpe::aes::openssl {
namespace engine {

template<typename ciph_type = aes::openssl::AesCtr128>
class AesCtrPRNG {
 public:
    using cipher_type = ciph_type;
    using ctx_type  = typename ciph_type::ctx_type;

    using   key_type = typename ctx_type::key_type;
    using    iv_type = typename ctx_type::iv_type;
    using block_type = typename ctx_type::block_type;

 private:
    ctx_type ctx;

 public:
    AesCtrPRNG(const key_type& key, const iv_type& iv)
      : ctx{key, iv}
    {  }

    AesCtrPRNG(uint8_t *key, uint8_t *iv)
      : ctx{key, iv}
    {  }

    AesCtrPRNG(AesCtrPRNG&&) noexcept = default;
    AesCtrPRNG& operator=(AesCtrPRNG&&) noexcept = default;


    void fill_block(block_type &array) noexcept;

    template<typename Container>
    void fill_bytes(Container &buffer) noexcept;

 protected:
    void fill_pointer(uint8_t *ptr) noexcept;
};


template<typename ciph_type>
template<typename Container>
void AesCtrPRNG<ciph_type>::fill_bytes(Container &buffer) noexcept {
    static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
    static constexpr arr_u8_t<ctx_type::BLOCK_BYTES_LEN> dummy_plaintext {};

    uint32_t copied = 0;
    while (copied + ctx_type::BLOCK_BYTES_LEN <= buffer.size()) {
        fill_pointer(buffer.data() + copied);
        copied += ctx_type::BLOCK_BYTES_LEN;
    }

    auto tmp = ciph_type::encrypt(ctx, dummy_plaintext);
    std::copy(tmp.begin(),
              tmp.begin() + (buffer.size() - copied),
              buffer.begin() + copied);
}


template<typename ciph_type>
void AesCtrPRNG<ciph_type>::fill_block(block_type &array) noexcept {
    fill_pointer(array.data());
}


template<typename ciph_type>
void AesCtrPRNG<ciph_type>::fill_pointer(uint8_t *ptr) noexcept {
    static const arr_u8_t<ctx_type::BLOCK_BYTES_LEN> dummy_plaintext {};
    ciph_type::encrypt(ctx, dummy_plaintext, ptr);
}

}  // namespace engine
}  // namespace grafpe::aes::openssl
