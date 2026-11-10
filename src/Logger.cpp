#include "Logger.h"
#include <iostream>
#include <mutex>

void Logger::Log(const std::string& message, LogLevel level)
{
    // Static mutex to guarantee thread-safe serialization to console output streams
    // across concurrent background IO and worker threads.
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);

    switch (level)
    {
    case LogLevel::Info:
        std::cout << "[INFO] " << message << std::endl;
        break;
    case LogLevel::Warning:
        std::cout << "[WARN] " << message << std::endl;
        break;
    case LogLevel::Error:
        std::cerr << "[ERROR] " << message << std::endl;
        break;
    }
}
