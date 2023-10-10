/*
 * author: Adam Budziak
 */

#include <algorithm>
#include <sstream>
#include <string>

#include <pur/logger/Logger.hpp>

#include "../catch/catch.hpp"

struct tools {
    static std::string tolower(const std::string& s) {
        std::string res(s.size(), 0);
        std::transform(s.begin(), s.end(), res.begin(), ::tolower);
        return res;
    }

    static bool contains(const std::string& msg, const std::string& pat) {
        return msg.find(pat) != std::string::npos;
    }
};

TEST_CASE("Basic Logger tests") {
    std::ostringstream oss;
    auto logger = pur::logger::Logger::createNonColored(oss);

    auto tolower = tools::tolower;
    auto contains = tools::contains;

    logger.info("first message");
    REQUIRE(contains(oss.str(), "first message"));
    REQUIRE(contains(tolower(oss.str()), "info"));
    oss.str({});

    logger.warning("second message");
    REQUIRE(contains(oss.str(), "second message"));
    REQUIRE(contains(tolower(oss.str()), "warn"));
    oss.str({});

    logger.error("third message");
    REQUIRE(contains(oss.str(), "third message"));
    REQUIRE(contains(tolower(oss.str()), "error"));
    oss.str({});
}


TEST_CASE("Multi parameters Logger") {
    std::ostringstream oss;
    auto logger = pur::logger::Logger::createNonColored(oss);

    auto contains = tools::contains;

    logger.info("The", "variable", '=', 123);
    REQUIRE(contains(oss.str(), "The variable = 123"));
    oss.str({});

    logger.warning(144, 12, 0.123, true, "", false, "hello");
    REQUIRE(contains(oss.str(), "144 12 0.123 1  0 hello"));
}
