/*
 * author: Adam Budziak
 */

#pragma once

#include <string>
#include <map>
#include <vector>

#include "TokenizerBase.hpp"

namespace pur {

/**
 * Class for syntactic parsing of the input parameters
 *
 */
class Tokenizer : public TokenizerBase {
    tokens_t tokens;
    std::string last_key;

 public:
    Tokenizer():
        tokens{},
        READ_KEY{*this},
        READ_VAL{*this},
        READ_VALSTR{*this},
        current_state{&READ_KEY}
        {}


    Tokenizer(const Tokenizer&) = delete;
    Tokenizer(Tokenizer&&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;
    Tokenizer& operator=(Tokenizer&&) = delete;


    const tokens_t& tokenize(int argc, char **argv) override;
    void tokenize(const std::string& str) override;

    const tokens_t& get_tokens() const override;

 private:
    class state {
     protected:
        Tokenizer& t;
        explicit state(Tokenizer &t): t{t} {}

     public:
        virtual ~state() = default;
        virtual void parse(const std::string& str) = 0;
    };



    struct read_key : public state {
        explicit read_key(Tokenizer& t): state{t} {}
        void parse(const std::string& str) override;
    };



    struct read_val : public state {
        explicit read_val(Tokenizer& t): state{ t } {}
        void parse(const std::string& str) override;
    };



    struct read_valstr : public state {
        explicit read_valstr(Tokenizer& t): state { t } {}
        void parse(const std::string& str) override;

     private:
        std::string val_str;
    };

    read_key READ_KEY;
    read_val READ_VAL;
    read_valstr READ_VALSTR;

    state* current_state;

    void set_state(state* s);
};
}  // namespace pur

