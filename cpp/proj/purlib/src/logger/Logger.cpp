/*
 * author: Adam Budziak
 */

#include <pur/logger/Logger.hpp>

namespace pur::logger {

void Logger::get_time(char *time_str) const {
    std::time_t t = std::time(nullptr);
    std::strftime(time_str, 128, "%d.%m.%Y %H:%M:%S", std::localtime(&t));
}
}
