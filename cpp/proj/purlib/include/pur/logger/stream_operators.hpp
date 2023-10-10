/*
 * author: Adam Budziak
 */

#pragma once

#include <tuple>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>
#include <iostream>

namespace pur {
namespace stream_operators {

namespace quote_operators {
std::ostream& operator<<(std::ostream& os, char c);
}


template <typename T, typename = void>
struct is_std_container : std::false_type {};

template <typename...>
using void_t = void;


template<typename K, typename V>
std::ostream&
operator<<(std::ostream& os, const std::pair<K, V>& pair);

template <typename T>
struct is_std_container<T,
void_t<decltype(std::declval<T&>().begin()),
        decltype(std::declval<T&>().end()),
        std::enable_if_t<!std::is_convertible<T, std::string>(), void>,
        typename T::value_type
>>
: std::true_type {};



template<class Container>
typename std::enable_if<
        is_std_container<Container>::value,
        std::ostream&>::type
operator<<(std::ostream& os, const Container& ctr) {
    static const char * const SEP = "";
    const char *separator = SEP;

    os << '[';

    using value_t = typename Container::value_type;

    if constexpr (std::is_constructible_v<std::string, value_t> ||
                  std::is_convertible_v<value_t, std::string> ||
                  std::is_same_v<std::remove_reference_t<value_t>, char>) {
        os << '"';
        for (const auto& i : ctr) {
            os << separator << i;
            separator = "\", \"";
        }
        os << '"';
    } else {
        for (const auto& i : ctr) {
            os << separator << i;
            separator = ", ";
        }
    }

    os << ']';

    return os;
}


template<typename K, typename V,
        template<typename, typename...> class Map,
        typename ...Args>
    typename std::enable_if<
        is_std_container<Map<K, V, Args...>>::value
        && !std::is_same_v<Map<K, V, Args...>, std::vector<K, V>>,
        std::ostream&>::type
operator<<(std::ostream& os, const Map<K, V, Args...>& map) {
    using namespace quote_operators;
    static const char * const SEPARATOR = ", ";
    const char *sep = "";
    os << "{";

    for (const auto& [key, val] : map) {
        os << sep << key << ": " << val;
        sep = SEPARATOR;
    }

    os << "}";

    return os;
}

template<typename Tuple, size_t ...I>
std::ostream&
print_tuple_impl(std::ostream& os, const Tuple& tuple, std::index_sequence<I...>) {
    os << '(';
    (..., (os << (I == 0 ? "" : ", ") << std::get<I>(tuple)));
    os << ')';
    return os;
}

template<typename ...Types>
std::ostream&
operator<<(std::ostream& os, const std::tuple<Types...>& tuple) {
    return print_tuple_impl(os, tuple, std::make_index_sequence<sizeof...(Types)>());
}


template<typename K, typename V>
std::ostream&
operator<<(std::ostream& os, const std::pair<K, V>& pair) {
    os << '(' << pair.first << ", " << pair.second << ')';
    return os;
}

}  // namespace stream_operators
}  // namespace pur
