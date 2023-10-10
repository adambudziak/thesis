/*
 * author: Adam Budziak
 */

#pragma once

#include <memory>
#include <tuple>
#include <functional>

#include <grafpe/crypt/Crypter.hpp>

namespace grafpe::crypter {


namespace detail {

    template<typename ...Crypters, typename value_type, std::size_t ...I>
    constexpr value_type encrypt_impl(
            const std::tuple<Crypters...>& crypters, const value_type& params,
            std::index_sequence<I...>)
    noexcept {
        return {std::get<I>(crypters).encrypt(std::get<I>(params))...};
    }


    template<typename ...Crypters, typename value_type, std::size_t ...I>
    constexpr value_type decrypt_impl(
            const std::tuple<Crypters...>& crypters, const value_type& params,
            std::index_sequence<I...>)
    noexcept {
        return {std::get<I>(crypters).decrypt(std::get<I>(params))...};
    }

    template<typename ...Crypters, typename value_type, std::size_t ...I>
    constexpr value_type encrypt_impl(
            const std::tuple<Crypters...>& crypters, const value_type& params,
            std::index_sequence<I...> /* index */,
            const tweak_t& tweak)
    noexcept {
        return {std::get<I>(crypters).encrypt(std::get<I>(params), tweak)...};
    }

    template<typename ...Crypters, typename value_type, std::size_t ...I>
    constexpr value_type decrypt_impl(
            const std::tuple<Crypters...>& crypters, const value_type& params,
            std::index_sequence<I...> /* index */,
            const tweak_t& tweak)
    noexcept {
        return {std::get<I>(crypters).decrypt(std::get<I>(params), tweak)...};
    }
}  // namespace detail

/**
 * Crypter class used for encryption of whole datarows.
 *
 */
template <typename ...Crypters>
class Tuple: public Base<std::tuple<typename Crypters::value_type...>> {
 public:
    using value_type = std::tuple<typename Crypters::value_type...>;

    explicit Tuple(Crypters... crypters) noexcept : m_crypters {std::move(crypters)...}
    {  }

    using Base<value_type>::encrypt;
    using Base<value_type>::decrypt;

    [[nodiscard]]
    constexpr decltype(auto) encrypt(const typename Crypters::value_type& ...params) const {
        return encrypt(std::make_tuple(params...));
    }

    [[nodiscard]]
    constexpr decltype(auto) decrypt(const typename Crypters::value_type& ...params) const {
        return decrypt(std::make_tuple(params...));
    }

 private:
    constexpr value_type encrypt_impl(const value_type& tup, const tweak_t& tweak) const override {
        return detail::encrypt_impl(m_crypters, tup, typename std::make_index_sequence<sizeof ...(Crypters)>(), tweak);
    }

    constexpr value_type decrypt_impl(const value_type& tup, const tweak_t& tweak) const override {
        return detail::decrypt_impl(m_crypters, tup, typename std::make_index_sequence<sizeof ...(Crypters)>(), tweak);
    }

 private:
    std::tuple<Crypters...> m_crypters;
};

}  // namespace grafpe::crypter
