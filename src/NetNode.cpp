#include "NetNode.h"
#include <sstream>
#include <chrono>
#include <algorithm>

using asio::ip::tcp;

NetNode::NetNode(const std::string& name, unsigned short listen_port)
    : name_(name),
      listen_port_(listen_port),
      io_(),
      acceptor_(io_, tcp::endpoint(tcp::v4(), listen_port_))
{
    // keep the io_context alive until we explicitly reset the guard
    work_guard_ = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
        asio::make_work_guard(io_)
    );
}

NetNode::~NetNode()
{
    stop();
}

void NetNode::start()
{
    startAccept();
    io_thread_ = std::thread([this]() { runIO(); });
    Logger::Log(name_ + " listening on port " + std::to_string(listen_port_), LogLevel::Info);
}

void NetNode::stop() {
    // Drop the work guard so io_context can exit once all handlers complete
    work_guard_.reset();

    // Close acceptor to cancel pending accepts
    asio::post(io_, [this]()
    {
        asio::error_code ec;
        acceptor_.cancel(ec);
        acceptor_.close(ec);
    });

    // stop io and join thread
    io_.stop();
    if (io_thread_.joinable()) io_thread_.join();

    // close all peer sockets
    std::lock_guard<std::mutex> lock(peers_mutex_);
    for (auto& s : peer_sockets_)
    {
        if (s && s->is_open())
        {
            asio::error_code ec;
            s->shutdown(tcp::socket::shutdown_both, ec);
            s->close(ec);
        }
    }
    peer_sockets_.clear();

    Logger::Log(name_ + " stopped", LogLevel::Info);
}

void NetNode::runIO()
{
    try {
        io_.run();
    } catch (const std::exception& ex)
    {
        Logger::Log(std::string("IO exception: ") + ex.what(), LogLevel::Error);
    }
}

void NetNode::startAccept()
{
    // async accept - use lambda capturing this
    acceptor_.async_accept([this](asio::error_code ec, tcp::socket socket)
    {
        if (!ec)
        {
            onAccept(std::move(socket));
        } else
        {
            // If shutting down, accept may error; log at warning
            Logger::Log(std::string("accept error: ") + ec.message(), LogLevel::Warning);
        }
        // if acceptor still open, keep accepting
        if (acceptor_.is_open()) startAccept();
    });
}

void NetNode::onAccept(tcp::socket socket) {
    Logger::Log(name_ + " accepted incoming connection", LogLevel::Info);
    auto sock = std::make_shared<tcp::socket>(std::move(socket));
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        peer_sockets_.push_back(sock);
    }
    startRead(sock);
}

void NetNode::connectToPeer(const std::string& host, unsigned short port)
{
    // do connection on the io context so all sockets are owned by IO thread
    asio::post(io_, [this, host, port]()
    {
        try
        {
            tcp::resolver resolver(io_);
            auto endpoints = resolver.resolve(host, std::to_string(port));
            auto sock = std::make_shared<tcp::socket>(io_);
            asio::connect(*sock, endpoints);
            {
                std::lock_guard<std::mutex> lock(peers_mutex_);
                peer_sockets_.push_back(sock);
            }
            Logger::Log(name_ + " connected to peer " + host + ":" + std::to_string(port), LogLevel::Info);
            startRead(sock);
        }
        catch (const std::exception& ex)
        {
            Logger::Log(std::string("connect error: ") + ex.what(), LogLevel::Warning);
        }
    });
}

void NetNode::send(const Packet& pkt)
{
    auto sent_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                       pkt.timestamp.time_since_epoch())
                       .count();

    std::ostringstream oss;
    oss << pkt.id << '|' << pkt.source << '|' << pkt.destination << '|' << sent_ts << '\n';
    auto msg = oss.str();

    asio::post(io_, [this, msg]() {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (auto it = peer_sockets_.begin(); it != peer_sockets_.end(); ) {
            auto& s = *it;
            if (!s || !s->is_open()) {
                it = peer_sockets_.erase(it);
                continue;
            }

            asio::error_code ec;
            asio::write(*s, asio::buffer(msg), ec);

            if (ec) {
                Logger::Log(std::string("send error: ") + ec.message(), LogLevel::Warning);
                it = peer_sockets_.erase(it);
            } else {
                ++it;
            }
        }
    });
}

void NetNode::startRead(std::shared_ptr<tcp::socket> sock)
{
    // read until newline into a streambuf
    auto buf = std::make_shared<asio::streambuf>();
    asio::async_read_until(*sock, *buf, '\n',
        [this, sock, buf](asio::error_code ec, std::size_t bytes_transferred)
        {
            if (ec)
            {
                // socket closed or error
                if (ec == asio::error::eof || ec == asio::error::connection_reset)
                {
                    Logger::Log(name_ + " peer disconnected", LogLevel::Info);
                } else
                {
                    Logger::Log(std::string("read error: ") + ec.message(), LogLevel::Warning);
                }
                // remove socket
                std::lock_guard<std::mutex> lock(peers_mutex_);
                peer_sockets_.erase(std::remove_if(peer_sockets_.begin(), peer_sockets_.end(),
                    [sock](const std::shared_ptr<tcp::socket>& s)
                    {
                        return !s || !s->is_open() || s->native_handle() == sock->native_handle();
                    }), peer_sockets_.end());
                return;
            }

            std::istream is(buf.get());
            std::string line;
            std::getline(is, line);
            if (!line.empty())
            {
                // parse: id|source|destination|sent_ts
                std::istringstream iss(line);
                std::string token;
                int id = -1;
                std::string source, destination;
                long long sent_ts = 0;

                if (std::getline(iss, token, '|')) id = std::stoi(token);
                if (std::getline(iss, source, '|')) {}
                if (std::getline(iss, destination, '|')) {}
                if (std::getline(iss, token, '|')) sent_ts = std::stoll(token);

                // compute latency (ms) using system_clock
                auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::system_clock::now().time_since_epoch())
                                  .count();
                long long latency = now_ms - sent_ts;

                std::ostringstream log;
                log << "[" << name_ << "] Received packet " << id << " from " << source
                    << " (latency " << latency << " ms)";
                Logger::Log(log.str(), LogLevel::Info);
            }

            // continue reading from this socket
            startRead(sock);
        });
}
