/*
 * author: Adam Budziak
 */

#include <iostream>
#include <string>

#include <pur/argparser/ArgParser.hpp>
#include <pur/argparser/Argument.hpp>
#include "../catch/catch.hpp"

using pur::argparser::ArgParser;
using pur::argparser::Argument;

using namespace std::string_literals;


TEST_CASE("Arg parser register arguments") {
    ArgParser<pur::Tokenizer> parser{};

    SECTION("Registering by name") {
        parser.register_arg<int>("int");
        parser.register_arg<bool>("bool");
        parser.register_arg<std::string>("string");
        parser.register_arg<double>("double");

        REQUIRE(!parser.has_value("int"));
        REQUIRE(!parser.has_value("bool"));
        REQUIRE(!parser.has_value("string"));
        REQUIRE(!parser.has_value("double"));
    }

    SECTION("Registering from Argument object") {
        auto arg1 = Argument::create<bool>("v");

        parser.register_arg(arg1);
        REQUIRE(!parser.has_value("v"));

        parser.register_arg(Argument::create<int>("d"));
        REQUIRE(!parser.has_value("d"));
    }

    SECTION("Re-registering arguments") {
        using exc_t = pur::argparser::ArgumentRedefinitionExc;

        parser.register_arg<int>("n");

        REQUIRE_THROWS_AS(
                parser.register_arg<int>("n"),
                exc_t);

        REQUIRE_THROWS_AS(
                parser.register_arg(Argument::create<int>("n")),
                exc_t);
    }
}



TEST_CASE("argparser basic register-set-get arguments") {
    int argc = 4;
    const char *argv[] = {
            "main",
            "-n=5",
            "-v",
            "--eps=0.123"
    };

    pur::argparser::defaultParser parser{};

    parser.register_arg<int>("n");
    parser.register_arg<double>("eps");
    parser.register_arg<bool>("v");

    SECTION("Getting arguments' values") {
        parser.parse_program_params(argc, const_cast<char **>(argv));

        REQUIRE(parser.get_value<int>("n") == 5);
        REQUIRE(parser.get_value<double>("eps") == 0.123);
        REQUIRE(parser.get_value<bool>("v"));
    }

    SECTION("Setting arguments' values") {
        WHEN("Setting value for the correct type") {
            THEN("All is ok") {
                parser.set_value("n", 10);
                REQUIRE(parser.get_value<int>("n") == 10);

                parser.set_value("eps", .999);
                REQUIRE(parser.get_value<double>("eps") == .999);

                parser.set_value("v", true);
                REQUIRE(parser.get_value<bool>("v"));
            }

            WHEN("Getting value for an incorrect type") {
                THEN("An exception is thrown") {
                    using exc_t = pur::argparser::ArgumentInvalidTypeExc;

                    REQUIRE_THROWS_AS(parser.get_value<bool>("n"), exc_t);
                    REQUIRE_THROWS_AS(parser.get_value<int>("eps"), exc_t);
                }
            }
        }

        WHEN("Setting value for an incorrect type") {
            THEN("An exception is thrown") {
                using exc_t = pur::argparser::ArgumentInvalidTypeExc;
                REQUIRE_THROWS_AS(parser.set_value("n", true), exc_t);
                REQUIRE_THROWS_AS(parser.set_value("eps", "hello"), exc_t);
            }
        }
    }
}
