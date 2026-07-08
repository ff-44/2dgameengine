#include "Logger.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>

const char* TIMEFORMAT = "%d/%b/%Y %H:%M:%S";
const char* GREEN = "\033[32m";
const char* RED   = "\033[31m";
const char* RESET = "\033[0m";


void Logger::Log(const std::string& message) {
    // Print on the console message:
    // LOG: [12/Oct/2026 09:13:17] - Actual message
    // This should be displayed in green
    std::tm localTime = GetTime();

    std::cout << GREEN;
    std::cout << "LOG: ";
    std::cout << '[' << std::put_time(&localTime, TIMEFORMAT) << "] " << message << std::endl;
    std::cout << RESET;

}

void Logger::Err(const std::string& message) {
    // Print on the console message:
    // ERR: [12/Oct/2026 09:13:17] - Actual message
    // Should be displayed in red

    std::tm localTime = GetTime();

    std::cout << RED;
    std::cout << "ERR: ";
    std::cout << '[' << std::put_time(&localTime, TIMEFORMAT) << "] " << message << std::endl;
    std::cout << RESET;
}

std::tm Logger::GetTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime = *std::localtime(&time);

    return localTime;
}