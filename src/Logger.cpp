#include "Logger.h"

void Logger::Log(const std::string& message, LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        std::cout << "[INFO] " << message << std::endl;
        break;
    case LogLevel::Warning:
        std::cout << "[WARNING] " << message << std::endl;
        break;
    case LogLevel::Error:
        std::cerr << "[ERROR] " << message << std::endl;
        break;
    }
}
