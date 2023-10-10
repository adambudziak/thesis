/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <grafpe/common/type_utils.hpp>
#include <grafpe/crypt/Cast.hpp>
#include <algorithm>

namespace grafpe::rte {

/**
 * A crypter class used for encryption of homogeneous collections of items.
 *
 */
template<typename Cont, typename Crypter_T>
class Container: public crypter::CastCrypter<Crypter_T, typename Cont::value_type> {
    using Self = Container<Cont, Crypter_T>;
    Cont cont;

 public:
    using value_type = typename Cont::value_type;
    using index_type = typename Crypter_T::value_type;

    Container(Cont cont, Crypter_T crypter) noexcept
      : crypter::CastCrypter<Crypter_T, typename Cont::value_type>{std::move(crypter)}, cont{ std::move(cont) } {
        static_assert(std::is_integral_v<index_type>);
    }

    const Cont& container() const noexcept {
        return cont;
    }

    /**
     * Searches for the given value in the container.
     *
     */
    [[nodiscard]]
    constexpr decltype(auto) find(const value_type& value) const {
        return std::find(cont.begin(), cont.end(), value);
    }

 private:
    constexpr inline index_type get_index(const value_type& value) const {
        return static_cast<index_type>(find(value) - cont.begin());
    }

protected:
    index_type cast_impl(const value_type& value) const override {
        return get_index(value);
    }

    value_type uncast_impl(const index_type& value) const override {
        return cont.at(value);
    }
};

}  // namespace grafpe::rte
