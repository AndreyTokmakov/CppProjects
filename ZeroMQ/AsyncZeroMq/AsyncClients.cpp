/**============================================================================
Name        : AsyncClients.cpp
Created on  : 13.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsyncClients.cpp
============================================================================**/

#include "AsyncClients.hpp"

#include <azmq/socket.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <format>
#include <string_view>

namespace async_req_client
{
    namespace asio = boost::asio;

    class AsyncReqClient
    {
    public:
        explicit AsyncReqClient(asio::io_context& ioc)
            : zmqSocket(ioc, ZMQ_REQ)
            , timer(ioc)
            , counter(0)
        {
            zmqSocket.connect("tcp://127.0.0.1:5556");
        }

        void start() {
            sendRequest();
        }

    private:

        void sendRequest()
        {
            const std::string message = std::format("Request-{}", ++counter);

            // REQ-сокет: просто отправляем тело, конверт ZeroMQ добавит сам
            zmqSocket.async_send(asio::buffer(message), [this](const boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "Send error: " << ec.message() << "\n";
                    return;
                }
                receiveReply();
            });
        }

        void receiveReply()
        {
            // REQ-сокет: получаем только тело ответа, конверт отбрасывается автоматически
            zmqSocket.async_receive(asio::buffer(recvBuffer),[this](const boost::system::error_code ec, const std::size_t size) {
                if (ec) {
                    std::cerr << "Receive error: " << ec.message() << "\n";
                    return;
                }

                const std::string_view reply(recvBuffer.data(), size);
                std::cout << "Reply: " << reply << "\n";

                timer.expires_after(std::chrono::seconds(1));
                timer.async_wait([this](const boost::system::error_code err) {
                    if (!err) {
                        sendRequest();
                    }
                });
            });
        }

        std::array<char, 1024> recvBuffer{};
        azmq::socket zmqSocket;
        asio::steady_timer timer;
        uint64_t counter { 0 };
    };

    void run()
    {
        try
        {
            asio::io_context ioc;
            AsyncReqClient client(ioc);
            client.start();
            ioc.run();
        } catch (const std::exception& e) {
            std::cerr << "Fatal: " << e.what() << "\n";
        }
    }
}

void async_zmq_clients::testAll(const std::vector<std::string_view>& args)
{
    async_req_client::run();
}
