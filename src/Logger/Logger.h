#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <chrono>

class Logger {
    private:
        static std::tm GetTime();

    public:
        static void Log(const std::string& message);
        static void Err(const std::string& message);
};

#endif