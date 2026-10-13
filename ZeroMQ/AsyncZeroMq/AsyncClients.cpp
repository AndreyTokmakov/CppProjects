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
        explicit AsyncReqClient(asio::io_context& ioc):
            zmqSocket { ioc, ZMQ_REQ },
            timer { ioc },
            timeoutTimer { ioc },
            counter { 0 }
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
            // Взводим таймаут перед началом ожидания
            timeoutTimer.expires_after(std::chrono::seconds(3));
            timeoutTimer.async_wait([this](const boost::system::error_code ec)
            {
                if (ec == asio::error::operation_aborted) {
                    // Таймер отменён — ответ пришёл вовремя, ничего не делаем
                    return;
                }
                if (!ec) {
                    // Таймер сработал — ответ не пришёл за 3 секунды
                    std::cerr << "Timeout: no reply within 3s\n";
                    handleTimeout();
                }
            });

            zmqSocket.async_receive(asio::buffer(recvBuffer),[this](
                const boost::system::error_code ec, const std::size_t size)
            {
                // Ответ пришёл — отменяем таймаут
                timeoutTimer.cancel();
                if (ec)
                {
                    if (ec == asio::error::operation_aborted) {
                        // Сокет отменён (например, при завершении) — просто выходим
                        return;
                    }
                    std::cerr << "Receive error: " << ec.message() << "\n";
                    return;
                }

                const std::string_view reply{recvBuffer.data(), size};
                std::cout << "Reply: " << reply << "\n";

                scheduleNext();
            });
        }

        void handleTimeout()
        {
            // Вариант A: просто попробовать ещё раз (пересоздать соединение — сложно)
            // Вариант B: перепланировать запрос с задержкой
            // Вариант C: пересоздать сокет
            // Ниже — вариант B: ждём секунду и пробуем снова
            scheduleNext();
        }

        void scheduleNext()
        {
            timer.expires_after(std::chrono::milliseconds(250));
            timer.async_wait([this](const boost::system::error_code ec) {
                if (!ec) sendRequest();
            });
        }

        std::array<char, 1024> recvBuffer{};
        azmq::socket zmqSocket;
        asio::steady_timer timer;         // пауза между запросами
        asio::steady_timer timeoutTimer;  // ожидание ответа
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


#if 0
#include "AsyncClients.hpp"

#include <azmq/socket.hpp>
#include <boost/asio.hpp>

#include <array>
#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace async_dealer_client
{
    namespace asio = boost::asio;
    using namespace std::chrono_literals;

    class AsyncDealerClient
    {
    public:
        explicit AsyncDealerClient(asio::io_context& ioc)
            : zmqSocket(ioc, ZMQ_DEALER)
            , timer(ioc)
            , timeoutTimer(ioc)
            , counter(0)
        {
            zmqSocket.connect("tcp://127.0.0.1:5556");
        }

        void start()
        {
            sendRequest();
        }

    private:
        void sendRequest()
        {
            const std::string message = std::format("Request-{}", ++counter);
            std::cout << "Sending: " << message << "\n";

            // DEALER 需要手动加空帧作为 REP 的界定符
            // 使用 shared_ptr 保证消息在异步发送完成前有效
            auto body = std::make_shared<std::string>(message);

            // 第一步：发送空帧（REP 分隔符）
            zmqSocket.async_send(asio::buffer("", 0),
                [this, body](const boost::system::error_code ec, std::size_t) {
                    if (ec) {
                        std::cerr << "Send delimiter error: " << ec.message() << "\n";
                        scheduleNext();
                        return;
                    }
                    // 第二步：发送请求体
                    zmqSocket.async_send(asio::buffer(*body),
                        [this, body](const boost::system::error_code ec2, std::size_t) {
                            if (ec2) {
                                std::cerr << "Send body error: " << ec2.message() << "\n";
                                scheduleNext();
                                return;
                            }
                            receiveReply();
                        });
                });
        }

        void receiveReply()
        {
            // 启动超时定时器
            timeoutTimer.expires_after(3s);
            timeoutTimer.async_wait([this](const boost::system::error_code ec) {
                if (ec == asio::error::operation_aborted) {
                    // 被取消：说明回复在超时前到达，正常继续
                    return;
                }
                if (!ec) {
                    // 超时触发：清理状态，直接调度下一次请求
                    std::cerr << "Timeout: no reply within 3s, retrying...\n";
                    // DEALER 状态干净，无需重建 socket
                    // 直接安排下一次请求即可
                    scheduleNext();
                }
            });

            // 等待回复：DEALER 先读空帧，再读正文
            readEnvelope();
        }

        void readEnvelope()
        {
            auto buf = std::make_shared<std::array<char, 256>>();

            zmqSocket.async_receive(asio::buffer(*buf),
                [this, buf](const boost::system::error_code ec, std::size_t size) {
                    if (ec) {
                        // 如果是超时触发的取消，不需要报错
                        if (ec != asio::error::operation_aborted) {
                            std::cerr << "Receive envelope error: " << ec.message() << "\n";
                        }
                        return;
                    }

                    // 取消超时定时器（回复已到达）
                    timeoutTimer.cancel();

                    // 继续读正文
                    readBody();
                });
        }

        void readBody()
        {
            auto buf = std::make_shared<std::array<char, 1024>>();

            zmqSocket.async_receive(asio::buffer(*buf),
                [this, buf](const boost::system::error_code ec, std::size_t size) {
                    if (ec) {
                        if (ec != asio::error::operation_aborted) {
                            std::cerr << "Receive body error: " << ec.message() << "\n";
                        }
                        return;
                    }

                    const std::string_view reply(buf->data(), size);
                    std::cout << "Reply: " << reply << "\n";

                    scheduleNext();
                });
        }

        void scheduleNext()
        {
            timer.expires_after(1s);
            timer.async_wait([this](const boost::system::error_code ec) {
                if (!ec) {
                    sendRequest();
                }
            });
        }

        azmq::socket zmqSocket;
        asio::steady_timer timer;
        asio::steady_timer timeoutTimer;
        std::array<char, 1024> recvBuffer{};
        uint64_t counter{0};
    };

    void run()
    {
        try
        {
            asio::io_context ioc;
            AsyncDealerClient client(ioc);
            client.start();
            ioc.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Fatal: " << e.what() << "\n";
        }
    }
}

void async_zmq_clients::testAll(const std::vector<std::string_view>& args)
{
    async_dealer_client::run();
}

#endif