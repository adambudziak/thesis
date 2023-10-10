/*
 * author: Adam Budziak
 */

#pragma once

#include <utility>
#include <iostream>
#include <ctime>
#include <string>
#include <chrono>

#include "stream_operators.hpp"


namespace pur {
namespace logger {

using stream_operators::operator<<;

struct colors {
    static constexpr auto BLACK = "\033[1;30m";
    static constexpr auto RED = "\033[1;31m";
    static constexpr auto GREEN = "\033[1;32m";
    static constexpr auto YELLOW = "\033[1;33m";
    static constexpr auto BLUE = "\033[1;34m";
    static constexpr auto MAGENTA = "\033[1;35m";
    static constexpr auto CYAN = "\033[1;36m";
    static constexpr auto WHITE = "\033[1;37m";
    static constexpr auto RESET = "\033[0m";
};

struct config {
    bool colored;
    std::string
            info_color,
            warning_color,
            error_color;

    std::ostream *os;

    config() = delete;

    static config Colored(std::ostream &os) {
        return config{
                true,
                colors::BLUE,
                colors::YELLOW,
                colors::RED,
                &os
        };
    }

    static config NonColored(std::ostream &os) {
        return config{
                false, "", "", "", &os
        };
    }
};

// TODO(abudziak) make it thread-safe

class Logger {
    static constexpr size_t FMTLEN = 32;

    config conf;

    Logger() : conf{config::NonColored(std::cout)} {}
    explicit Logger(config&& conf) : conf{std::move(conf)} {}

 public:
    using color_t = std::string;

    static Logger createColored(std::ostream& os) {
        return Logger{config::Colored(os)};
    }

    static Logger createNonColored(std::ostream& os) {
        return Logger{config::NonColored(os)};
    }

    void set_ostream(std::ostream &os) {
        conf.os = &os;
    }

    template<typename ... MsgT>
    void info(const MsgT &... msg);


    template<typename ... MsgT>
    void warning(const MsgT &... msg);


    template<typename ... MsgT>
    void error(const MsgT &... msg);

 private:
    template<typename ... MsgT>
    void log_msg(const color_t &color,
                 const std::string &type,
                 const MsgT &... msg);


    void get_time(char *) const;


    template<typename Head, typename ... Tail>
    void unwrap_args(const Head &h, const Tail &... t);


    template<typename Last>
    void unwrap_args(const Last &l);
};


template<typename ... MsgT>
void Logger::info(const MsgT &... msg) {
    log_msg(conf.info_color, "INFO", msg...);
}

template<typename ... MsgT>
void Logger::warning(const MsgT &... msg) {
    log_msg(conf.warning_color, "WARNING", msg...);
}

template<typename ... MsgT>
void Logger::error(const MsgT &... msg) {
    log_msg(conf.error_color, "ERROR", msg...);
}

template<typename ... MsgT>
void Logger::log_msg(const color_t &color,
                     const std::string &type,
                     const MsgT &... msg) {
    char time[FMTLEN];
    get_time(time);
    *conf.os << color << "[" << time << "] "
       << type << ": " << colors::RESET << "  ";

    unwrap_args(msg...);

    *conf.os << std::endl;
}

template<typename Head, typename... Tail>
void Logger::unwrap_args(const Head &h, const Tail &... t) {
    *conf.os << h << ' ';
    unwrap_args(t...);
}

template<typename Last>
void Logger::unwrap_args(const Last &l) {
    *conf.os << l;
}


}  // namespace logger
}  // namespace pur
