/*
 * author: Adam Budziak
 */

#include <iostream>
#include <cassert>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/aes/openssl/AesCtr128PRNG.hpp>
#include <grafpe/common/print_utils.hpp>
#include <dfare/dfa/RankedDFA.hpp>
#include <grafpe/rte/DFA.hpp>
#include <grafpe/rte/Container.hpp>
#include <grafpe/crypt/Tuple.hpp>

using namespace grafpe;
using namespace std::string_literals;
using grafpe::aes::openssl::AesCtr128PRNG;

template <typename Crypter_T>
class NameCrypter: public crypter::CastCrypter<Crypter_T, std::string> {
    dfare::dfa::RankedDFA m_dfa;
public:
    explicit NameCrypter(dfare::dfa::RankedDFA dfa, Crypter_T crypter):
        crypter::CastCrypter<Crypter_T, std::string>(std::move(crypter)),
        m_dfa {std::move(dfa)} {}

protected:
    perm_t cast_impl(const std::string &value) const override {
        perm_t casted(value.size());
        std::transform(value.begin(), value.end(), casted.begin(),
                [this](char c){ return m_dfa.rank(std::string{c});});
        return casted;
    }

    std::string uncast_impl(const perm_t &value) const override {
        std::string uncasted(value.size(), '\0');
        std::transform(value.begin(), value.end(), uncasted.begin(),
                [this](point_t p){ return m_dfa.unrank(p)[0];});
        return uncasted;
    }
};

template <typename key_type, typename iv_type>
auto _prepare_crypter(const key_type& key, const iv_type& iv) {
    using ScalarGrafpe = crypter::ScalarGrafpe<AesCtr128PRNG>;
    using string_vec = std::vector<std::string>;

    const auto domains = string_vec {
        "gmail", "wp", "onet", "interia", "protonmail", "pudelek", "yahoo", "hotmail"
    };

    const auto extensions = string_vec {
        "pl", "edu", "com", "net", "eu", "en"
    };

    auto domains_crypter = rte::Container{
        domains,
        ScalarGrafpe{key, iv, domains.size(), 2, 100}
    };

    auto extensions_crypter = rte::Container{
        extensions,
        ScalarGrafpe{key, iv, extensions.size(), 2, 100}
    };

    const std::string name_regex = "[a-z]";

    auto ranked_dfa = dfare::dfa::RankedDFA::from_string(name_regex, 1);

    auto name_crypter = NameCrypter{
        ranked_dfa,
        crypter::Grafpe<>{key, iv, ranked_dfa.max_rank, 4u, 100u}
    };

    return crypter::Tuple{name_crypter, domains_crypter, extensions_crypter};
}

template <typename Crypter_T>
class EmailCrypter: public crypter::CastCrypter<Crypter_T, std::string> {
 public:
    using value_type = std::string;

    explicit EmailCrypter(Crypter_T crypter):
        crypter::CastCrypter<Crypter_T, std::string>{std::move(crypter)} {  }

 private:
    using tuple_t = typename Crypter_T::value_type;

protected:
    tuple_t cast_impl(const value_type& value) const override {
        std::size_t at_pos { value.find('@') };
        std::size_t dot_pos { value.find('.', at_pos)};
        return std::tuple {
                value.substr(0, at_pos),
                value.substr(at_pos + 1, dot_pos - at_pos - 1),
                value.substr(dot_pos + 1)
        };
    }

    std::string uncast_impl(const tuple_t& tuple) const override {
        return std::get<0>(tuple) + '@' + std::get<1>(tuple) + '.' + std::get<2>(tuple);
    }
};

int main() {
    auto key = random_array<AesCtr128PRNG::key_type>(DummyPRNG{});
    auto  iv = random_array<AesCtr128PRNG::iv_type>(DummyPRNG{});

    std::cout << "Key = " << bytes_to_hex(key) << '\n'
              << " IV = " << bytes_to_hex(iv) << '\n';

    EmailCrypter crypter{_prepare_crypter(key, iv)};

    auto plaintext = "helloworld@gmail.com"s;
    std::cout << plaintext << '\n'
              << crypter.encrypt(plaintext) << '\n'
              << crypter.decrypt(crypter.encrypt(plaintext)) << '\n';
}
