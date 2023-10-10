/*
 * author: Adam Budziak
 */

#pragma once

#include <cstring>
#include <iostream>
#include <utility>

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/common/print_utils.hpp>

#include "GraphTraverser.hpp"
#include "Crypter.hpp"

namespace grafpe::crypter {

/**
 *
 * Default Zn <=> Zn encryption/decryption class.
 *
 * It uses the most straightforward way of encryption,
 * basically identical to the algorithm described in the protocol.
 *
 * The encryption key should have the same format as the one used in prng_engine_t.
 *
 * The encrypted values are vectors of std::uint64_t.
 * Thic class can be used for encryption of single values by wrapping them
 * into 1-element vectors. However,in this case the ScalarGrafpe class should be used.
 *
 */
template<typename prng_engine_t = aes::openssl::AesCtr128PRNG>
class Grafpe: public Base<perm_t> {
    using ctx_t = typename prng_engine_t::ctx_type;

 public:
    using key_type = typename ctx_t::key_type;
    using iv_type = typename ctx_t::iv_type;

 private:
    GraphTraverser traverser;
    key_type key;

 public:
    Grafpe(Graph &&graph, size_t walk_length, key_type key)
      : traverser{ std::move(graph), walk_length },
        key { std::move(key) }  { }

    Grafpe(key_type key, iv_type iv, point_t N, std::size_t D, std::size_t walk_length)
    : traverser{GraphBuilder<prng_engine_t>::create_from_engine({key, iv}).build(N, D), walk_length},
    key { std::move(key) } {  }

    size_t size() const;

 private:
    perm_t encrypt_impl(const perm_t &msg, const tweak_t &tweak) const override;
    perm_t decrypt_impl(const perm_t &crypt, const tweak_t &tweak) const override;
};

template<typename prng_engine_t>
perm_t Grafpe<prng_engine_t>::encrypt_impl(const perm_t &msg, const tweak_t &tweak) const {
    if (msg.empty()) return {};

    perm_t crypt(msg.size());
    perm_t absorbed(msg.size());

    key_type key_ {};
    iv_type   iv {};

    vec_u8_t random(traverser.getContext().walk_length * (traverser.getContext().f + 1));

    const auto encrypt_ = [&](const perm_t &src, perm_t &tgt) {
        point_t x0 = 0;
        for (size_t i = 0; i < tgt.size(); ++i) {
            generate_key_iv<ctx_t>(get_digest(src, tgt, i), tweak, key_, iv);
            prng_engine_t{key, iv}.fill_bytes(random);
            tgt[i] = traverser.go((src[i] + x0) % traverser.getContext().n, random);
            x0 = tgt[i];
        }
    };

    encrypt_(msg, absorbed);
    encrypt_(absorbed, crypt);

    return crypt;
}


template<typename prng_engine_t>
perm_t Grafpe<prng_engine_t>::decrypt_impl(const perm_t &crypt, const tweak_t &tweak) const {
    if (crypt.empty()) { return {}; }

    perm_t msg(crypt.size());
    perm_t absorbed(crypt.size());

    key_type key_ {};
    iv_type iv {};

    vec_u8_t random(traverser.getContext().walk_length * (traverser.getContext().f + 1));

    const auto decrypt_ = [&](const perm_t& src, perm_t& tgt) {
        for (size_t i = src.size()-1; i > 0; --i) {
            generate_key_iv<ctx_t>(get_digest(tgt, src, i), tweak, key_, iv);
            prng_engine_t{key, iv}.fill_bytes(random);
            tgt[i] = (traverser.go_back(src[i], random) + traverser.getContext().n - src[i-1])
                        % traverser.getContext().n;
        }
        generate_key_iv<ctx_t>(get_digest(tgt, src, 0), tweak, key_, iv);
        prng_engine_t{key, iv}.fill_bytes(random);
        tgt[0] = traverser.go_back(src[0], random);
    };


    decrypt_(crypt, absorbed);
    decrypt_(absorbed, msg);

    return msg;
}

template<typename prng_engine_t>
size_t Grafpe<prng_engine_t>::size() const {
    return traverser.getContext().n;
}

}  // namespace grafpe::crypter
