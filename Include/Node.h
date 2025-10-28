#pragma once
#include "Packet.h"
#include "Logger.h"
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class Node {
public:
    explicit Node(std::string name);
    void connect(Node* other);
    void send(Packet pkt);
    void start();
    void stop();

private:
    void processPackets();

    std::string name_;
    Node* connectedNode_;
    std::queue<Packet> inbox_;
    std::mutex mutex_;
    std::atomic<bool> running_;
    std::thread worker_;
};
