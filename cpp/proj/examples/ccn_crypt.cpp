/*
 * author: Adam Budziak
 */

#include <chrono>

#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/crypt/Cast.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/crypt/Targeted.hpp>
#include <grafpe/domain/CycleWalker.hpp>
#include <cassert>
#include <grafpe/domain/CycleSlicer.hpp>

using namespace grafpe;

static constexpr uint64_t BASE = 10000;

bool luhn_checksum(uint64_t number) {
    static const auto NUM_SIZE = static_cast<uint64_t>(pow(10, 15));

    if (number < NUM_SIZE || number >= NUM_SIZE * 10) {
        return false;
    }

    uint64_t sum = 0;
    uint64_t even = 0;
    while (number != 0) {
        auto doubled = ((number % 10) * (even + 1));
        sum += doubled >= 10 ? doubled - 9 : doubled;
        number /= 10;
        even = 1 - even;
    }
    return sum % 10 == 0;
}

/**
 * Class able to perform enryption on 16-digit numbers
 * @tparam Crypter_T
 */
template <typename Crypter_T>
class DGT16Crypter: public crypter::CastCrypter<Crypter_T, uint64_t> {
public:
    explicit DGT16Crypter(Crypter_T crypter): crypter::CastCrypter<Crypter_T, uint64_t> {crypter} {}
protected:
   perm_t cast_impl(const uint64_t &value_) const override {
        auto value = value_;
        perm_t ccn(4);
        for (int i = 3; i >= 0; --i) {
            ccn[i] = value % BASE;
            value /= BASE;
        }
        return ccn;
    }

    uint64_t uncast_impl(const perm_t &value) const override {
        uint64_t ccn = 0;
        for (int i = 0; i < 4; ++i) {
            ccn *= BASE;
            ccn += value[i];
        }
        return ccn;
    }
};

int main() {
    using prng_t = aes::openssl::AesCtr128PRNG;

    auto key = random_array<prng_t::key_type>(DummyPRNG{});
    auto  iv = random_array<prng_t::iv_type>(DummyPRNG{});

    std::cout << "CCN crypter random key and iv:\n"
        << '\t' << bytes_to_hex(key) << '\n'
        << '\t' << bytes_to_hex(iv) << '\n';

    // encryption of 16-digit credit card numbers
    // Step 1: treat the number AAAA BBBB CCCC DDDD as a vector of
    //  4 digits in base 10'000

    auto dgt16_crypter = DGT16Crypter {
        crypter::Grafpe<> { key, iv, BASE, 4, 432}
    };

    auto ccn_crypter = crypter::Targeted {
        domain::targeting::CycleWalker<decltype(dgt16_crypter)>{&luhn_checksum},
//        domain::targeting::CycleSlicer<decltype(dgt16_crypter)>{{key, iv}, [](auto& a, auto& b) { return luhn_checksum(a) && luhn_checksum(b);}, 100},
        dgt16_crypter
    };

    using namespace std::chrono;

    std::cout << std::boolalpha;

    double benchmark = 0;
    uint64_t start = 1'000'000'000'000'000;
    uint64_t total = 2000;
    for (uint64_t i = start; i < start + total; ++i) {
        if (!luhn_checksum(i)) { continue; }
        auto start = high_resolution_clock::now();
        auto encrypted = ccn_crypter.encrypt(i);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        benchmark += duration / double(total);
    }

    std::cout << "CCN benchmark: " << benchmark  << '\n';

    std::cout << luhn_checksum(1234123412341238) << std::endl;

    std::cout << ccn_crypter.encrypt(1234123412341238) << std::endl;
}
