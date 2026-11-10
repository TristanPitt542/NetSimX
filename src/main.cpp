#include "NetNode.h"
#include "Logger.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    Logger::Log("Starting NetSimX initialization...", LogLevel::Info);

    // Mode 1: Zero-argument execution runs a self-contained 2-node loopback integration test.
    // Ideal for quick local validation and out-of-the-box demonstration.
    if (argc == 1)
    {
        Logger::Log("Running internal 2-node loopback integration demo...", LogLevel::Info);

        NetNode nodeA("NodeA", 9001);
        NetNode nodeB("NodeB", 9002);

        // Start background IO threads and binding acceptors
        nodeA.start();
        nodeB.start();

        // Brief delay to ensure sockets are fully listening before connection handshake
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Establish bi-directional peer connections
        nodeA.connectToPeer("127.0.0.1", 9002);
        nodeB.connectToPeer("127.0.0.1", 9001);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Simulate a burst of interleaved packet transmissions
        for (int i = 0; i < 5; ++i)
        {
            Packet p1{i, "NodeA", "NodeB", std::chrono::system_clock::now()};
            Packet p2{i, "NodeB", "NodeA", std::chrono::system_clock::now()};
            
            nodeA.send(p1);
            nodeB.send(p2);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Allow time for final packet processing and latency logging
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Graceful teardown of network nodes and IO contexts
        nodeA.stop();
        nodeB.stop();
    }
    // Mode 2: Parameterized execution for distributed multi-process testing
    else if (argc == 5)
    {
        try 
        {
            std::string name = argv[1];
            unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));
            std::string peer_host = argv[3];
            unsigned short peer_port = static_cast<unsigned short>(std::stoi(argv[4]));

            Logger::Log("Launching single node instance [" + name + "]...", LogLevel::Info);

            NetNode node(name, port);
            node.start();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            node.connectToPeer(peer_host, peer_port);

            // Transmit periodic telemetry packets to the target peer
            for (int i = 0; i < 10; ++i)
            {
                Packet p{i, name, "peer", std::chrono::system_clock::now()};
                node.send(p);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            node.stop();
        }
        catch (const std::exception& ex)
        {
            Logger::Log(std::string("CLI argument parsing or runtime error: ") + ex.what(), LogLevel::Error);
            std::cerr << "Usage Error. Check arguments and port ranges.\n";
            return 1;
        }
    } 
    else
    {
        // Print usage instructions if argument count is invalid
        std::cout << "NetSimX Usage:\n"
                  << "  NetSimX                              # Run built-in 2-node loopback demo\n"
                  << "  NetSimX <name> <port> <peer_host> <peer_port>  # Run standalone node instance\n";
    }

    Logger::Log("NetSimX execution completed successfully.", LogLevel::Info);
    return 0;
}
