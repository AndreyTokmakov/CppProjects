/**============================================================================
Name        : PollingReqRspServer.cpp
Created on  : 13.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PollingReqRspServer.cpp
============================================================================**/

#include "Servers.hpp"

#include <zmq.hpp>

#include <atomic>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>


namespace
{
    class PollingReqRepServer
    {
    public:
        PollingReqRepServer(const std::string_view ipAddress, const std::uint16_t port):
            context {1 },
            zmqSocket { context, zmq::socket_type::rep },
            endpoint { std::format("tcp://{}:{}", ipAddress, port)}
        {
        }

        void run()
        {
            zmq::pollitem_t items[] = {{
                .socket = zmqSocket,
                .fd = 0,
                .events = ZMQ_POLLIN,
                .revents = 0
            }};

            zmq::message_t message;
            std::string response;
            response.reserve(ResponseBufferCapacity);
            try
            {
                zmqSocket.bind(endpoint);
                std::cout << "Server listening on " << endpoint << '\n';

                while (true)
                {
                    const int events = zmq::poll(items, 1, PollTimeout);
                    if (events == 0 || (items[0].revents & ZMQ_POLLIN) == 0) {
                        continue;
                    }

                    const zmq::recv_result_t received = zmqSocket.recv(message, zmq::recv_flags::none);
                    if (!received.has_value()) {
                        continue;
                    }

                    const std::string_view request { static_cast<const char*>(message.data()),message.size() };
                    if (!processRequest(request, response)) {
                        std::cout << "Failed to process request\n";
                    }
                    sendResponse(response);
                }
            } catch (const zmq::error_t& error) {
                std::cerr << "ZeroMQ error: " << error.what() << '\n';
            } catch (const std::exception& error) {
                std::cerr << "Server error: " << error.what() << '\n';
            }
        }

    private:

        [[nodiscard]]
        std::optional<std::string> receiveRequest()
        {
            zmq::message_t message;
            const zmq::recv_result_t received = zmqSocket.recv(message, zmq::recv_flags::none);
            if (!received.has_value()) {
                return std::nullopt;
            }

            return std::string { static_cast<const char*>(message.data()), message.size()};
        }

        [[nodiscard]]
        bool processRequest(const std::string_view request, std::string& response)
        {
            std::cout << "Received: " << request << '\n';
            response.clear();
            response.append("Response: ");
            response.append(request);
            return true;
        }

        void sendResponse(const std::string& response)
        {
            const zmq::send_result_t sent = zmqSocket.send(zmq::buffer(response), zmq::send_flags::none);
            if (!sent.has_value()) {
                throw std::runtime_error{"Failed to send response"};
            }

            std::cout << "Sent: " << response << '\n';
        }

        static constexpr std::size_t ResponseBufferCapacity { 1024 };
        static constexpr std::chrono::milliseconds PollTimeout { 100 };

        zmq::context_t context;
        zmq::socket_t zmqSocket;
        std::string endpoint;
    };

    [[nodiscard]]
    std::optional<std::uint16_t> parsePort(const std::string_view value)
    {
        unsigned int port = 0;
        const auto [ptr, error] = std::from_chars(value.data(), value.data() + value.size(), port);
        if (error != std::errc{} || ptr != value.data() + value.size() || port == 0 || port > 65535) {
            return std::nullopt;
        }

        return static_cast<std::uint16_t>(port);
    }
}


void Servers::runPollingReqRepServer(const std::vector<std::string_view>& args)
{
    constexpr std::string_view DefaultIpAddress = "0.0.0.0";
    constexpr std::uint16_t DefaultPort = 5556;

    const std::string_view ipAddress = args.empty() ? DefaultIpAddress : args.front();
    const std::optional<uint16_t> port = args.size() < 2 ? std::optional{DefaultPort} : parsePort(args[1]);

    PollingReqRepServer server { ipAddress, port.value_or(DefaultPort) };
    server.run();
}

