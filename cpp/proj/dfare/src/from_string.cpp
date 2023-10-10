/*
 *  author: Adam Budziak
 */

#include <string>
#include <stack>
#include <algorithm>

#include <exception>

#include <pur/logger/stream_operators.hpp>

#include <dfare/regexp/from_string.hpp>
#include <dfare/regexp/RegExp.hpp>
#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Closure.hpp>
#include <dfare/exceptions.hpp>
#include <dfare/common/Alphabet.hpp>

namespace dfare::regexp {

constexpr char OPERATORS[] {"()+.*"};

class pop_empty_stack: public std::exception {
 public:
    const char* what() const noexcept override {
        return "Pop from an empty stack";
    }
};

class top_empty_stack: public std::exception {
 public:
    const char* what() const noexcept override {
        return "Top an empty stack";
    }
};

inline bool is_operator(char c) {
    return std::find(std::begin(OPERATORS), std::end(OPERATORS), c) != std::end(OPERATORS);
}

inline bool lower_priority_than(char left, char right) {
    auto pos = std::find(std::begin(OPERATORS), std::end(OPERATORS), left);
    return std::find(pos, std::end(OPERATORS), right) != std::end(OPERATORS);
}

template<typename T>
inline constexpr T top(const std::stack<T>& stack) {
    if (stack.empty()) { throw top_empty_stack {}; }
    return stack.top();
}

template<typename T>
inline constexpr T pop(std::stack<T>& stack) {
    if (stack.empty()) { throw pop_empty_stack {}; }
    auto top = stack.top();
    stack.pop();
    return top;
}

std::string insert_operators(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());

    std::string result;
    bool needs_concat = false;
    for (size_t i = 0; i < str.size() - 1; ++i) {
        if (needs_concat) { result += '.'; }
        result += str[i];
        needs_concat = (str[i] == '*' && !is_operator(str[i+1]))
                || (!is_operator(str[i]) && (!is_operator(str[i+1]) || str[i+1] == '('))
                || (str[i] == ')' && (str[i+1] == '(' || !is_operator(str[i+1])))
                || (i > 0 && str[i-1] == '\\');
    }
    if (needs_concat) { result += '.'; }
    result += str[str.size()-1];
    return result;
}

std::string infix_to_postfix(const std::string& str) {
    std::stack<char> stack;
    std::string postfix;

    using pur::stream_operators::operator<<;
    for (std::size_t i = 0; i < str.size(); ++i) {
         switch (str[i]) {
             case '\\':
                 if (i == str.size()-1) {
                     throw invalid_regexp_string{"trailing backslash"};
                 }
                 postfix += '\\';
                 postfix += str[i+1];
                 ++i;
                 break;
             case ')':
                 while (top(stack) != '(') {
                     postfix += pop(stack);
                 }
                 pop(stack);
                 break;
             case '.':
             case '+':
                 while (!stack.empty() && !lower_priority_than(top(stack), str[i])) {
                     postfix += pop(stack);
                 }
             case '(':
                 stack.push(str[i]);
                 break;
             default:
                 postfix += str[i];
                 break;
        }
    }
    while (!stack.empty()) { postfix += pop(stack); }
    return postfix;
}

std::string preprocess(const std::string& str_) {
    std::string result;
    std::size_t i = 0;
    for(; i < str_.size(); ++i) {
        if (str_[i] == '[') {
            if (str_.size() < i + 5) {
                throw invalid_regexp_string{"Invalid regex " + str_};
            }
            auto start = str_[i+1];
            auto end = str_[i+3];
            if (str_[i+2] != '-' || start > end) {
                break;
            }
            result += '(';
            result += start;
            ++start;
            for(; start <= end; ++start) {
                result += '+';
                result += start;
            }
            result += ')';
            i += 4;
        } else if(str_[i] == '.') {
            result += "\\.";
        } else {
            result += str_[i];
        }
    }
    if (i != str_.size()) {
        throw invalid_regexp_string{"Invalid regex" + str_};
    }
    return result;
}

regexp_ptr from_string_(const std::string& str_) {
    std::stack<regexp_ptr> stack;
    std::cout << preprocess(str_) << '\n';
    auto str = insert_operators(preprocess(str_));
    std::cout << str_ << '\n';
    auto postfix = infix_to_postfix(str);
    std::cout << postfix << '\n';

    for (std::size_t i = 0; i < postfix.size(); ++i) {
        if (!is_operator(postfix[i])) {
            if (postfix[i] == '\\') {
                stack.push(RegExp::create(postfix[i+1]));
                ++i;
            } else {
                stack.push(RegExp::create(postfix[i]));
            }
        } else {
            auto top = pop(stack);
            if (postfix[i] == '+') {
                stack.push(Sum::create(pop(stack), top));
            } else if (postfix[i] == '.') {
                stack.push(Concat::create(pop(stack), top));
            } else {
                stack.push(Closure::create(top));
            }
        }
    }

    return top(stack);
}

dfare::Alphabet get_alphabet(const std::string& str) {
    std::string result{};
    for(std::size_t i = 0; i < str.size(); ++i) {
        if (!is_operator(str[i])) {
            result += str[i];
        }
        if (str[i] == '\\' && is_operator(str.at(i+1))) {
            result += str[i+1];
        }
    }
    return dfare::Alphabet{result};
}


regexp_ptr from_string(const std::string& str) {
    regexp_ptr result;
    try {
        result = from_string_(str);
    } catch(const pop_empty_stack&) {
        throw invalid_regexp_string{str};
    } catch(const top_empty_stack&) {
        throw invalid_regexp_string{str};
    }
    return result;
}

}  // namespace dfare::regexp
