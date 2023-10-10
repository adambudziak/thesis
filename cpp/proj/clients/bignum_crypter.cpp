/*
 * author: Adam Budziak
 */

#include <iostream>

#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/common/print_utils.hpp>


int main(int argc, char **argv) {
    using namespace grafpe;
    using engine_t = aes::openssl::AesCtr128PRNG;
    using Crypter = crypter::Grafpe<aes::openssl::AesCtr128PRNG>;

    uint64_t number;
    if (argc < 2) {
        std::cout << "Number not specified, type N: ";
        std::cin >> number;
    } else {
        try {
            number = std::stoul(argv[1]);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Couldn't parse number, quitting." << std::endl;
            exit(1);
        }
    }

    const auto key = array_from_string<Crypter::key_type>("0123012301230123");
    const auto  iv = array_from_string<Crypter::iv_type>("3210321032103210");

    auto crypter = Crypter{key, iv, 10, 4, 64};

    perm_t plaintext{};

    do {
        plaintext.insert(plaintext.begin(), 1, static_cast<perm_t::value_type>(number % 10));
        number /= 10;
    } while (number);

    auto crypt = crypter.encrypt(plaintext);

    perm_t::value_type crypt_num = 0;

    for (auto num : crypt) {
        crypt_num = crypt_num*10 + num;
    }


    std::cout << "Encrypted: " << crypt_num << std::endl;

    for (auto num : crypter.decrypt(crypt)) {
        number = number*10 + num;
    }

    std::cout << "Decrypted: " << number << std::endl;
}

