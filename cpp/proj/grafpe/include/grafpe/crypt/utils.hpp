/*
 * author: Adam Budziak
 */

#pragma once

#include <cmath>
#include <cstring>
#include <utility>

#include <openssl/evp.h>

#include <grafpe/common/type_utils.hpp>
#include <grafpe/graph/Graph.hpp>
#include <grafpe/permutation/utils.hpp>
#include <iostream>

namespace grafpe {

/**
 * A helper POD holding all constant context required to perform
 * encryption/decryption on a single graph.
 */
struct CryptCtx {
    uint64_t n;
    uint64_t d;
    int f;
    size_t walk_length;
    perm_vec_t permutations;
    perm_vec_t inv_permutations;

    CryptCtx(Graph&& graph, size_t walk_length)
    : n{graph.n},
      d{graph.d},
      f{static_cast<int>(log2(graph.d)+1)},
      walk_length {walk_length},
      permutations{std::move(graph.permutations)},
      inv_permutations{inverse_permutations(permutations)} {}
};


vec_u8_t get_digest(
        const perm_t& xs,
        const perm_t& ys,
        size_t index);

/**
 * Extracts i-th bit from the buffer of bytes.
 * Accessing bits out-of-range is UB.
 * @param buffer - a vector of bytes
 * @param i - index of the bit to extract
 * @return bool representing the extracted bit
 */
bool get_bit(const vec_u8_t &buffer, size_t i);

uint32_t get_bits(uint32_t source, uint32_t start, uint32_t end);
uint32_t get_bits(const vec_u8_t &buffer, uint32_t start, uint32_t end);

bool get_direction(const vec_u8_t &buffer, int bit_width, size_t i);

uint32_t get_permutation_index(const vec_u8_t &buffer, int bit_width, size_t i);

/**
 * Selects the proper permutation as well as it's direction from the buffer.
 * Permutation index is given by bits_cnt bits starting at ith position.
 * The next bit represents the direction of the permutation.
 *
 * The function is faster than using get_direction and get_permutation_index
 * and should work equivalently.
 * @param buffer - buffer to extract the permutation and direction from.
 * @param bits_cnt - number of bits that represent the index of the permutation
 * @param i - index of the first bit in the buffer
 * @return pair, where the first is the index of the permutation,
 *               and the second is its direction.
 */
std::pair<uint32_t, bool> get_permutation(const vec_u8_t &buffer, int bits_cnt, size_t i);



/**
 * Given the digest of any length, generates a pseudo random key and iv,
 * that can be later used for encryption/decryption.
 *
 * The key and iv can be specified to differ from the types appropriate to given
 * encryption protocol, but if the length of the buffer is shorter than expected
 * then it's UB.
 *
 * @tparam cipher_t Type of the cipher to generate the KEY and IV for
 * @param digest Bytes that work as seed for the generator. The longer the better.
 * @param salt 64bit long salt
 * @param key an array of bytes to store the resulting KEY
 * @param iv an array of bytes to store the resulting IV
 */
template<typename cipher_t,
         typename key_t = typename cipher_t::key_type,
         typename iv_t = typename cipher_t::iv_type,
         typename Container = vec_u8_t>
void generate_key_iv(const Container &data_, const tweak_t &salt, key_t &key, iv_t &iv) {
    static_assert(sizeof(typename Container::value_type) == 1);
    static auto* dummy_data = reinterpret_cast<const unsigned char *>("0");

    int size;
    const unsigned char* data;
    // TODO(abudziak) temporary for backwards compatibility
    const unsigned char* salt_ = *salt.data() ? reinterpret_cast<const uint8_t*>(salt.data()) : nullptr;

    if (data_.data()) {
        data = data_.data();
        size = static_cast<int>(data_.size());
    } else {
        data = dummy_data;
        size = 1;
    }

    auto cipher = EVP_get_cipherbyname(cipher_t::NAME);
    auto dgst = EVP_get_digestbyname("md5");

    EVP_BytesToKey(cipher, dgst, salt_, data, size, 0, key.data(), iv.data());
}

template<typename cipher_t>
void generate_key_iv(const vec_u8_t &data_, typename cipher_t::key_type &key, typename cipher_t::iv_type &iv) {
    generate_key_iv<cipher_t>(data_, {}, key, iv);
}

}  // namespace grafpe
