/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <exception>
#include <string>

namespace pur {
namespace argparser {

class ArgumentRedefinitionExc : public std::exception {
    std::string msg;

 public:
    explicit ArgumentRedefinitionExc(std::string what)
    : msg {std::move(what)} {}

    const char * what() const noexcept override { return msg.data(); }
};


class ArgumentInvalidTypeExc : public std::exception {
    std::string msg;

 public:
    explicit ArgumentInvalidTypeExc(std::string what)
            : msg { std::move(what) } {}

    const char * what() const noexcept override { return msg.data(); }
};


class InvalidArgumentExc : public std::exception {
    std::string msg;

 public:
    explicit InvalidArgumentExc(std::string what)
             : msg { std::move(what) } {}

     const char * what() const noexcept override { return msg.data(); }
};

}  // namespace argparser

}  // namespace pur
