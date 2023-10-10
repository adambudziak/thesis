/*
 * author: Adam Budziak
 */

#include <grafpe/rte/Container.hpp>
#include <grafpe/crypt/Grafpe.hpp>
#include <grafpe/prng/utils.hpp>
#include <grafpe/prng/DummyPRNG.hpp>
#include <grafpe/crypt/ScalarGrafpe.hpp>
#include "../catch/catch.hpp"

TEST_CASE("Basic test of container crypter") {
    using namespace grafpe;
    using namespace aes::openssl;
    using Grafpe = crypter::ScalarGrafpe<AesCtr128PRNG>;

    auto cont = std::vector{"Burger", "Pasibus", "Burrito", "Pizza", "Ramen", "Spaghetti"};
    using val_t = typename decltype(cont)::value_type;

    rte::Container cont_crypter {
        cont,
        Grafpe{
            hex_to_array<AesCtr128::key_type>("6306296DB6C6FE5E534AC6742EE04713"),
            hex_to_array<AesCtr128::iv_type>("734971FF22AFD5185159CB2CDD2D6509"),
            cont.size(),
            4,
            100}
    };

    decltype(cont) results(cont.size());
    std::transform(cont.begin(), cont.end(), results.begin(), [&cont_crypter](const val_t & val) -> val_t {
        return cont_crypter.encrypt(val);
    });

    REQUIRE(results == decltype(cont){
        "Pizza", "Burger", "Ramen", "Pasibus", "Spaghetti", "Burrito"
    });
}
