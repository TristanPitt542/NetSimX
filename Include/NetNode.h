#pragma once

#include "Packet.h"
#include "Logger.h"

#include <asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <string>

class NetNode {
public:
    // name used for logs, listen_port is the TCP server port
    NetNode(const std::string& name, unsigned short listen_port);
    ~NetNode();

    // start acceptor + IO thread
    void start();

    // stop everything and join
    void stop();

    // connect to a remote peer (non-blocking; performed on io_context)
    void connectToPeer(const std::string& host, unsigned short port);

    // broadcast packet to connected peers
    void send(const Packet& pkt);

private:
    void runIO();
    void startAccept();
    void onAccept(asio::ip::tcp::socket socket);
    void startRead(std::shared_ptr<asio::ip::tcp::socket> sock);

    std::string name_;
    unsigned short listen_port_;

    asio::io_context io_;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;
    std::thread io_thread_;
    asio::ip::tcp::acceptor acceptor_;

    std::mutex peers_mutex_;
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> peer_sockets_;
};
