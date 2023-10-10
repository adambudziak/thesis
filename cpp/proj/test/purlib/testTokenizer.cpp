/*
 * author: Adam Budziak
 */

#include <iostream>
#include <string>

#include <pur/optional/Optional.hpp>
#include <pur/tokenizer/Tokenizer.hpp>

#include "../catch/catch.hpp"

TEST_CASE("Test of the pur::Optional") {
    SECTION("Empty optionals")  {
        pur::Optional<bool> opt{};
        REQUIRE(!opt.has_value());
        REQUIRE(!pur::Optional<std::string>{}.has_value());
    }
}

TEST_CASE("Tokenizer parses single Argument separated by space") {
    pur::Tokenizer t;
    t.tokenize("--output-file");
    t.tokenize("some_file.txt");

    auto tokens = t.get_tokens();

    REQUIRE(tokens.at("output-file").value() == "some_file.txt");
}

TEST_CASE("Tokenizer parses multiple arguments separated by spaces") {
    pur::Tokenizer t;
    t.tokenize("--output-file");
    t.tokenize("some_file.txt");
    t.tokenize("-n");
    t.tokenize("4");
    t.tokenize("-d");
    t.tokenize("8");

    auto tokens = t.get_tokens();
    REQUIRE(tokens.at("output-file").value() == "some_file.txt");
    REQUIRE(tokens.at("n").value() == "4");
    REQUIRE(tokens.at("d").value() == "8");
}

TEST_CASE("Tokenizer parses value-less arguments") {
    pur::Tokenizer t;
    SECTION("Tokenizer parses single valueless") {
        t.tokenize("--verbose");
        auto tokens = t.get_tokens();
        REQUIRE(!tokens.at("verbose").has_value());
    }
    SECTION("Tokenizer parses multiple valueless") {
        t.tokenize("-a");
        t.tokenize("-b");
        t.tokenize("-c");
        auto tokens = t.get_tokens();
        auto empty = [&tokens](const std::string& name) -> bool {
            return !tokens.at(name).has_value();
        };
        REQUIRE(empty("a"));
        REQUIRE(empty("b"));
        REQUIRE(empty("c"));
    }
}

TEST_CASE("Tokenizer parses both valued and valueless arguments") {
    pur::Tokenizer t;
    t.tokenize("--output-file");
    t.tokenize("some_file.txt");
    t.tokenize("--verbose");
    t.tokenize("-n");
    t.tokenize("4");
    t.tokenize("--quiet");
    auto tokens = t.get_tokens();
    REQUIRE(tokens.at("output-file").value() == "some_file.txt");
    REQUIRE(tokens.at("n").value() == "4");
    REQUIRE(!tokens.at("verbose").has_value());
    REQUIRE(!tokens.at("quiet").has_value());
}

TEST_CASE("Tokenizer parses arguments separated by =") {
    pur::Tokenizer t;
    t.tokenize("--output-file=some_file.txt");
    t.tokenize("-v=8");
    REQUIRE(t.get_tokens().at("v").value() == "8");
}

TEST_CASE("Tokenizer parses arguments separated both by = and space") {
    pur::Tokenizer t;
    t.tokenize("--output-file=some_file.txt");
    t.tokenize("-v=8");
    t.tokenize("-d");
    t.tokenize("4");
    REQUIRE(t.get_tokens().at("output-file").value() == "some_file.txt");
    REQUIRE(t.get_tokens().at("v").value() == "8");
    REQUIRE(t.get_tokens().at("d").value() == "4");
}

TEST_CASE("Tokenizer parses string-values with spaces") {
    pur::Tokenizer t;
    t.tokenize("--message");
    t.tokenize(R"("Hello, world!")");
    t.tokenize(R"(--response="Hello my friend")");
    auto tokens = t.get_tokens();
    REQUIRE(tokens.at("message").value() == "Hello, world!");
    REQUIRE(tokens.at("response").value() == "Hello my friend");
}


TEST_CASE("Tokenizer parses program input-parameters") {
    const char *argv[] = {
            "program_name",
            "--eps=0.123",
            R"(--message="Hello world!")",
            "-v",
            R"(--response="Hello my friend!")"
    };

    int argc = sizeof(argv) / sizeof(char*);

    pur::Tokenizer t;
    auto tokens = t.tokenize(argc, const_cast<char **>(argv));
    REQUIRE(tokens.find("program_name") == tokens.end());
    REQUIRE(tokens.at("eps").value() == "0.123");
    REQUIRE(tokens.at("message").value() == "Hello world!");
    REQUIRE(!tokens.at("v").has_value());
    REQUIRE(tokens.at("response").value() == "Hello my friend!");
}
