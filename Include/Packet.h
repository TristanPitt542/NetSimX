#pragma once
#include <string>
#include <chrono>

struct Packet {
    int id;
    std::string source;
    std::string destination;
    // use system_clock so timestamps are comparable across processes
    std::chrono::system_clock::time_point timestamp;
};
