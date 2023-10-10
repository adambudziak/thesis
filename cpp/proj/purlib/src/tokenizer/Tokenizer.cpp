/*
 * author: Adam Budziak
 */

#include <string>
#include <algorithm>
#include <iostream>

#include <pur/tokenizer/Tokenizer.hpp>

namespace pur {

// TODO(abudziak) how to allow the client to specify the rules by himself?
void Tokenizer::tokenize(const std::string& str) {
    current_state->parse(str);
}

const Tokenizer::tokens_t& Tokenizer::tokenize(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        tokenize(argv[i]);
    }
    return this->tokens;
}


const Tokenizer::tokens_t& Tokenizer::get_tokens() const {
    return tokens;
}

void Tokenizer::set_state(state* s) {
    current_state = s;
}



void Tokenizer::read_key::parse(const std::string& str) {
    auto pos = str.find_first_of('=');
    if (pos != std::string::npos) {
        t.tokenize(str.substr(0, pos));
        t.tokenize(str.substr(pos+1));
        return;
    }

    if (str.substr(0, 2) == "--") {
        t.last_key = str.substr(2);
    } else if (str[0] == '-') {
        t.last_key = str.substr(1);
    } else {
        t.last_key = str;
    }
    t.tokens[t.last_key] = Optional<bool>{};
    t.set_state(&t.READ_VAL);
}



void Tokenizer::read_val::parse(const std::string& str) {
    if (str[0] == '"') {
        t.set_state(&t.READ_VALSTR);
        t.tokenize(str);
        return;
    }
    if (str[0] == '-') {
        t.set_state(&t.READ_KEY);
        t.tokenize(str);
        return;
    }
    t.tokens[t.last_key] = str;
    t.set_state(&t.READ_KEY);
}



void Tokenizer::read_valstr::parse(const std::string& str) {
    auto pos = str.find_first_of('"');
    if (pos == 0) {
        val_str = "";
        t.tokenize(str.substr(1));
    }
    if (pos != std::string::npos) {
        val_str += str.substr(0, pos);
        t.tokens[t.last_key] = val_str;
        t.set_state(&t.READ_KEY);
    } else {
        val_str += str + ' ';
    }
}

}  // namespace pur
