#pragma once
#include <string>
#include <chrono>

struct Packet {
    int id;
    std::string source;
    std::string destination;
    std::chrono::steady_clock::time_point timestamp;
};
