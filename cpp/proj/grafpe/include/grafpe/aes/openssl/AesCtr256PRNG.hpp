/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/prng/PRNG.hpp>
#include "AesCtrPRNG_engine.hpp"
#include "AesCtr256.hpp"


namespace grafpe::aes::openssl {

class AesCtr256PRNG : public engine::AesCtrPRNG<AesCtr256>,
                      public PRNG_<AesCtr256PRNG, engine::AesCtrPRNG<AesCtr256>::ctx_type> {
 public:
    using engine::AesCtrPRNG<AesCtr256>::AesCtrPRNG;
    using engine::AesCtrPRNG<AesCtr256>::fill_block;
    using engine::AesCtrPRNG<AesCtr256>::fill_bytes;
};

class AesCtr256PRNG_MT : public engine::AesCtrPRNG<AesCtr256>,
                         public PRNG_MT_<AesCtr256PRNG_MT, engine::AesCtrPRNG<AesCtr256>::ctx_type> {
 public:
    using engine::AesCtrPRNG<AesCtr256>::AesCtrPRNG;
    using engine::AesCtrPRNG<AesCtr256>::fill_block;
    using engine::AesCtrPRNG<AesCtr256>::fill_bytes;
};

}  // namespace grafpe::aes::openssl

