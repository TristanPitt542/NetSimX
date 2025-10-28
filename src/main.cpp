#include "NetNode.h"
#include "Logger.h"
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    Logger::Log("Starting NetSimX (networked) ...", LogLevel::Info);

    if (argc == 1)
    {
        // run two nodes in same process
        NetNode nodeA("NodeA", 9001);
        NetNode nodeB("NodeB", 9002);

        // strt nodes
        nodeA.start();
        nodeB.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        //connect nodes
        nodeA.connectToPeer("127.0.0.1", 9002);
        nodeB.connectToPeer("127.0.0.1", 9001);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        //send 5 pac
        for (int i = 0; i < 5; ++i)
        {
            Packet p1{i, "NodeA", "NodeB", std::chrono::system_clock::now()};
            Packet p2{i, "NodeB", "NodeA", std::chrono::system_clock::now()};
            nodeA.send(p1);
            nodeB.send(p2);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        nodeA.stop();
        nodeB.stop();
    }
    else if (argc == 5)
    {
        std::string name = argv[1];
        unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));
        std::string peer_host = argv[3];
        unsigned short peer_port = static_cast<unsigned short>(std::stoi(argv[4]));

        NetNode node(name, port);
        node.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        node.connectToPeer(peer_host, peer_port);

        // send a few packets from this node to peer
        for (int i = 0; i < 10; ++i)
        {
            Packet p{i, name, "peer", std::chrono::system_clock::now()};
            node.send(p);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        node.stop();
    } else
    {
        std::cout << "Usage:\n"
                  << "  NetSimX            # run internal 2-node demo\n"
                  << "  NetSimX name port peer_host peer_port  # run single node instance\n";
    }

    Logger::Log("NetSimX finished.", LogLevel::Info);
    return 0;
}
