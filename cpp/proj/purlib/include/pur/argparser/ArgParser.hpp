/*
 * author: Adam Budziak
 */

#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <map>

#include <pur/argparser/Argument.hpp>
#include <pur/tokenizer/Tokenizer.hpp>
#include <pur/logger/stream_operators.hpp>

namespace pur {
namespace argparser {

template<typename Tokenizer = Tokenizer>
class ArgParser {
    using args_map = std::map<std::string, Argument>;

    std::map<std::string, Argument> args;

 public:
    template<typename T>
    void register_arg(const std::string& name);

    void register_arg(const Argument& arg);

    const args_map& get_args() const { return args; }

    bool has_argument(const std::string& name) const {
        return args.find(name) != args.end();
    }

    template<typename T>
    void set_value(const std::string &name, const T &val);

    template<typename T>
    const T& get_value(const std::string &name) const;

    bool has_value(const std::string &key) const;

    void parse_program_params(int argc, char **argv);
};


template <typename Tokenizer>
void ArgParser<Tokenizer>::register_arg(const Argument &arg) {
    if (has_argument(arg.get_name())) {
        throw ArgumentRedefinitionExc(
                "Argument named "s + arg.get_name() + " redefinition");
    }
    args.insert({arg.get_name(), arg});
}


template<typename Tokenizer>
template<typename T>
void ArgParser<Tokenizer>::register_arg(const std::string &name) {
    if (has_argument(name)) {
        throw ArgumentRedefinitionExc(
                "Argument named "s + name + " redefinition");
    }
    args.insert({name, Argument::create<T>(name)});
}


template<typename Tokenizer>
template<typename T>
void ArgParser<Tokenizer>::set_value(const std::string &name, const T &val) {
    args.at(name). template set_value<T>(val);
}


template<typename Tokenizer>
template<typename T>
const T &ArgParser<Tokenizer>::get_value(const std::string &name) const {
    return args.at(name). template get_value<T>();
}


template<typename Tokenizer>
void ArgParser<Tokenizer>::parse_program_params(int argc, char **argv) {
    Tokenizer tok;
    tok.tokenize(argc, argv);
    for (const auto &[name, value] : tok.get_tokens()) {
        try {
            auto &arg = args.at(name);
            if (arg.is_flag() == !value.has_value()) {
                if (arg.is_flag()) {
                    arg.set_present(true);
                } else {
                    arg.set_value_from_string(value.value());
                }
            } else {
                // TODO(abudziak) make it some reasonable exception (arg_parser_invalid_argument)
                throw;
            }
        } catch (const std::out_of_range& exc) {
            throw InvalidArgumentExc("Invalid argument named " + name);
        }
    }
}


template<typename Tokenizer>
bool ArgParser<Tokenizer>::has_value(const std::string &key) const {
    return args.find(key) != args.end() && args.at(key).has_value();
}

using defaultParser = ArgParser<Tokenizer>;

}  // namespace argparser

}  // namespace pur
