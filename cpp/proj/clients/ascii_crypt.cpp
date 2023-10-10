/*
 * author: Adam Budziak
 */

#include <memory>
#include <utility>
#include <iostream>
#include <string>
#include <variant>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/aes/openssl/AesCtr256PRNG.hpp>
#include <grafpe/graph/GraphBuilder.hpp>
#include <grafpe/common/print_utils.hpp>
#include <grafpe/crypt/Grafpe.hpp>

using grafpe::bytes_to_hex, grafpe::vec_from_string;
using grafpe::vec_u8_t, grafpe::perm_t;
using namespace grafpe;
using namespace grafpe::aes::openssl;

template<typename key_type, typename iv_type>
std::pair<key_type, iv_type> read_key_iv() {
    std::string key_str, iv_str;

    std::cout << "Key: ";
    std::cin >> key_str;
    std::cout << "IV: ";
    std::cin >> iv_str;

    if (!grafpe::is_hex(key_str) || !grafpe::is_hex(iv_str)) {
        throw std::runtime_error("Key or iv is not a proper hexstring");
    }

    return {grafpe::hex_to_array<key_type>(key_str), grafpe::hex_to_array<iv_type>(iv_str)};
}

std::shared_ptr<crypter::Base<perm_t>> prepare_crypter() {
    std::string key_str, iv_str, cipher_engine;
    std::cout << "Cipher engine [aes-ctr-128]: ";
    std::cin >> cipher_engine;
    std::shared_ptr<crypter::Base<perm_t>> crypter;

    const point_t N = 256;
    const std::size_t D = 8;


    if (cipher_engine == "aes-ctr-128") {
        using engine_t = grafpe::aes::openssl::AesCtr128PRNG;
        decltype(read_key_iv<engine_t::key_type, engine_t::iv_type>()) key_iv;
        try {
            key_iv = read_key_iv<engine_t::key_type, engine_t::iv_type>();
        } catch(...) {
            std::cerr << "Invalid key or iv. Quitting." << std::endl;
            exit(1);
        }
        auto[key, iv] = key_iv;

        crypter = crypter::create<crypter::Grafpe<engine_t>>(key, iv, N, D, 20);
    }
    return crypter;
}

int main() {
    auto crypter = prepare_crypter();

    std::string plaintext;
    std::cout << "Message: ";
    std::cin.ignore();
    std::getline(std::cin, plaintext);
    std::cout << "Message: "
              << bytes_to_hex(vec_from_string<vec_u8_t>(plaintext))
              << std::endl;

    auto enciphered = crypter->encrypt(vec_from_string<perm_t>(plaintext));

    std::cout << "Enciphered: "
              <<  bytes_to_hex(vec_u8_t{enciphered.begin(), enciphered.end()})
              << std::endl;


    auto deciphered = crypter->decrypt(enciphered);

    std::cout << "Deciphered: "
              << bytes_to_hex(vec_u8_t{deciphered.begin(), deciphered.end()})
              << std::endl;

    std::cout << "Deciphered: " << std::string{deciphered.begin(), deciphered.end()} << std::endl;
}
