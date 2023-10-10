/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <variant>
#include <string>
#include <ostream>
#include "exceptions.hpp"
#include <pur/optional/Optional.hpp>

namespace pur {
namespace argparser {
using namespace std::string_literals;

class Argument {
    using arg_t = std::variant<bool, int, double, std::string>;

    template<typename T>
    constexpr static bool is_valid_t() {
        return std::is_same_v<bool, T>
                || std::is_same_v<int, T>
                || std::is_same_v<double, T>
                || std::is_same_v<std::string, T>;
    }

    template<typename T>
    decltype(auto) wrap_type(T&& val) {
        return std::forward<T>(val);
    }

    auto wrap_type(char const *str) {
        return std::string{str};
    }

    std::string name;
    arg_t arg;
    bool is_empty;

    explicit Argument(std::string name)
            : name{std::move(name)}, arg{}, is_empty{true} {}

 public:
    template<typename T>
    static
    typename std::enable_if<is_valid_t<T>(), Argument>::type
    create(const std::string& name) {
        Argument a{name};
        a.arg = T{};
        return a;
    }


    void set_value_from_string(const std::string& value) {
        if (std::holds_alternative<std::string>(arg)) {
            arg = value;
        } else if (std::holds_alternative<int>(arg)) {
            arg = std::stoi(value);
        } else if (std::holds_alternative<double>(arg)) {
            arg = std::stod(value);
        }
        is_empty = false;
    }

    template<typename T>
    void set_value(const T& value_) {
        auto value = wrap_type(value_);
        if (!std::holds_alternative<decltype(value)>(arg)) {
            throw ArgumentInvalidTypeExc(
                    "Trying to set a value "s
                    + " for " + name + " which holds a different type");
        }
        arg = value;
        is_empty = false;
    }


    template<typename T>
    const T& get_value() const {
        if (!std::holds_alternative<T>(arg)) {
            throw ArgumentInvalidTypeExc(
                    "Trying to get the value "s
                    + "for " + name + " which holds a different type");
        }
        return std::get<T>(arg);
    }

    const std::string& get_name() const {
        return name;
    }

    bool has_value() const {
        return !is_empty;
    }

    void set_present(bool present) {
        arg = present;
    }

    bool is_flag() const {
        return std::holds_alternative<bool>(arg);
    }

    friend std::ostream &operator<<(std::ostream &os, const Argument &arg) {
        os << "Argument(" << arg.name << ": ";
        std::visit([&os](auto &&arg) {  os << arg; }, arg.arg);
        os << ", empty: " << !arg.has_value() << ")";
        return os;
    }
};


}  // namespace argparser


}  // namespace pur
