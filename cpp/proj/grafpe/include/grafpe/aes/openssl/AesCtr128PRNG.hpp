/*
 * author: Adam Budziak
 */

#pragma once

#include <array>

#include <grafpe/prng/PRNG.hpp>
#include "AesCtrPRNG_engine.hpp"
#include "AesCtr128.hpp"


namespace grafpe::aes::openssl {


class AesCtr128PRNG : public engine::AesCtrPRNG<>,
                      public PRNG_<AesCtr128PRNG, engine::AesCtrPRNG<>::ctx_type> {
 public:
    using engine::AesCtrPRNG<>::AesCtrPRNG;
    using engine::AesCtrPRNG<>::fill_block;
    using engine::AesCtrPRNG<>::fill_bytes;
};

class AesCtr128PRNG_MT : public engine::AesCtrPRNG<>,
                         public PRNG_MT_<AesCtr128PRNG_MT, engine::AesCtrPRNG<>::ctx_type> {
 public:
    using engine::AesCtrPRNG<>::AesCtrPRNG;
    using engine::AesCtrPRNG<>::fill_block;
    using engine::AesCtrPRNG<>::fill_bytes;
};

}  // namespace grafpe::aes::openssl
