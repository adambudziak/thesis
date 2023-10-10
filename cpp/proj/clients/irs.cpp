//
// Created by abudziak on 1/5/19.
//

#include <grafpe/common/print_utils.hpp>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/permutation/PermGen_Compat.hpp>


int main() {
    using namespace grafpe;
    using prng_t = grafpe::aes::openssl::AesCtr128PRNG;

    const auto key = hex_to_array<prng_t::key_type>("01230123012301230123012301230123");
    const auto  iv = hex_to_array<prng_t::iv_type>("00001111222233330000111122223333");

    using crypter_t = grafpe::aes::openssl::AesCtr128;

    auto crypter = crypter_t::ctx_type{key, iv};

    permutation::Generator_Compat<prng_t> generator { prng_t{key, iv} };

    using pur::stream_operators::operator<<;
    std::cout << generator.create_permutation(10) << std::endl;
}
