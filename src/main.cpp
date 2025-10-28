#include "Node.h"
#include "Logger.h"
#include <thread>
#include <chrono>

int main() {
    Logger::Log("Starting NetSimX simulation...", LogLevel::Info);

    //Create Nodes
    Node nodeA("NodeA");
    Node nodeB("NodeB");

    //Connect Node A and B
    nodeA.connect(&nodeB);
    nodeB.connect(&nodeA);

    //Start Nodes
    nodeA.start();
    nodeB.start();

    //Send 5 packets from Node A to Node B
    for (int i = 0; i < 5; ++i) {
        Packet pkt{i, "NodeA", "NodeB", std::chrono::steady_clock::now()};
        nodeA.send(pkt);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    //Stop Nodes
    nodeA.stop();
    nodeB.stop();

    Logger::Log("Simulation ended successfully.", LogLevel::Info);
}
