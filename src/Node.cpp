#include "Node.h"
#include <thread>
#include <chrono>

//Constructor
Node::Node(std::string name)
    : name_(std::move(name)), connectedNode_(nullptr), running_(false) {}

void Node::connect(Node* other) {
    connectedNode_ = other;
}

void Node::send(Packet pkt) {
    //Error if Node doesn't have connection
    if (!connectedNode_) {
        Logger::Log("[WARNING] " + name_ + " has no connection!", LogLevel::Warning);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(connectedNode_->mutex_);
        connectedNode_->inbox_.push(pkt);
    }
}


void Node::start() {
    running_ = true;
    worker_ = std::thread(&Node::processPackets, this);
}

void Node::stop() {
    running_ = false;
    if (worker_.joinable()) worker_.join();
}

void Node::processPackets() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::unique_lock<std::mutex> lock(mutex_);
        if (!inbox_.empty()) {
            Packet pkt = inbox_.front();
            inbox_.pop();
            lock.unlock();

            //calculate latancy now - timestamp
            auto latency = std::chrono::steady_clock::now() - pkt.timestamp;
            //Log that packet was recieved
            Logger::Log("[INFO] " + name_ + " received packet " + std::to_string(pkt.id) +
                        " from " + pkt.source +
                        " (latency " + std::to_string(
                            std::chrono::duration_cast<std::chrono::milliseconds>(latency).count()
                        ) + " ms)", LogLevel::Info);
        }
    }
}
