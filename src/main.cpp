#include "Node.h"
#include "Logger.h"
#include <thread>
#include <chrono>

int main() {
    Logger::Log("Starting NetSimX simulation...", LogLevel::Info);

    Node nodeA("NodeA");
    Node nodeB("NodeB");

    nodeA.connect(&nodeB);
    nodeB.connect(&nodeA);

    nodeA.start();
    nodeB.start();

    for (int i = 0; i < 5; ++i) {
        Packet pkt{i, "NodeA", "NodeB", std::chrono::steady_clock::now()};
        nodeA.send(pkt);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    nodeA.stop();
    nodeB.stop();

    Logger::Log("Simulation ended successfully.", LogLevel::Info);
}
