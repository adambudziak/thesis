/*
 * author: Adam Budziak
 */

#pragma once

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/crypt/Crypter.hpp>
#include "GraphTraverser.hpp"

namespace grafpe::crypter {

template <typename prng_engine_t = aes::openssl::AesCtr128PRNG>
class ScalarGrafpe: public Base<typename perm_t::value_type> {
    using ctx_t = typename prng_engine_t::ctx_type;

 public:
    using key_type = typename ctx_t::key_type;
    using iv_type = typename ctx_t::iv_type;
    using value_type = typename perm_t::value_type;

 private:
    GraphTraverser traverser;
    key_type key;

 public:
    ScalarGrafpe(Graph graph, size_t walk_length, key_type key)
      : traverser{ std::move(graph), walk_length },
        key { std::move(key) } {}

    ScalarGrafpe(key_type key, iv_type iv, point_t N, std::size_t D, std::size_t walk_length)
      : traverser{GraphBuilder<prng_engine_t>::create_from_engine({key, iv}).build(N, D), walk_length},
        key { std::move(key) } {  }

    std::size_t size() const noexcept { return traverser.getContext().n; }

 private:
    point_t encrypt_impl(const point_t& msg, const tweak_t& tweak) const override;
    point_t decrypt_impl(const point_t& cipher, const tweak_t& tweak) const override;
};



template<typename prng_engine_t>
point_t ScalarGrafpe<prng_engine_t>::encrypt_impl(const point_t& msg, const tweak_t &tweak) const {
    key_type key_ {};
    iv_type iv {};

    point_t absorbed = 0;

    vec_u8_t random(traverser.getContext().walk_length * (traverser.getContext().f + 1));

    generate_key_iv<ctx_t>({0}, tweak, key_, iv);
    prng_engine_t{key, iv}.fill_bytes(random);

    absorbed = traverser.go(msg % traverser.getContext().n, random);
    return traverser.go(absorbed % traverser.getContext().n, random);
}


template<typename prng_engine_t>
point_t ScalarGrafpe<prng_engine_t>::decrypt_impl(const point_t& cipher, const tweak_t &tweak) const {
    key_type key_ {};
    iv_type iv {};
    point_t absorbed = 0;

    vec_u8_t random(traverser.getContext().walk_length * (traverser.getContext().f + 1));

    generate_key_iv<ctx_t>({0}, tweak, key_, iv);
    prng_engine_t{key, iv}.fill_bytes(random);

    absorbed = traverser.go_back(cipher, random);
    return traverser.go_back(absorbed, random);
}

template<typename Crypter>
auto create(typename Crypter::key_type key, typename Crypter::iv_type iv, point_t N, std::size_t D, std::size_t walk_length) {
    return std::make_shared<Crypter>(std::move(key), std::move(iv), N, D, walk_length);
}

}  // namespace grafpe::crypter
