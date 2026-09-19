/**============================================================================
Name        : Router_Dealer.h
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer.h
============================================================================**/

#include "Router_Dealer.hpp"
#include "Logger.hpp"
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>


namespace
{
    using namespace std::string_view_literals;
    constexpr std::string_view connString { "tcp://0.0.0.0:5556"sv };

    void server(logger::Logger& logger)
    {
        zmq::context_t ctx(1);
        zmq::socket_t router(ctx, zmq::socket_type::router);
        router.bind(connString.data());

        logger.info("[server] ROUTER bound on {}", connString);
        while (true)
        {
            // 1. Получаем identity
            zmq::message_t identity;
            const auto r1 = router.recv(identity, zmq::recv_flags::none);
            if (!r1)
                break;

            // 2. Пустой фрейм-разделитель (envelope)
            zmq::message_t delimiter;
            router.recv(delimiter, zmq::recv_flags::none);

            // 3. Payload
            zmq::message_t payload;
            router.recv(payload, zmq::recv_flags::none);

            std::string req(static_cast<char*>(payload.data()), payload.size());
            logger.info("[server] from {}", std::string(static_cast<char*>(identity.data()), identity.size()));

            // Имитация обработки
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            std::string reply = "echo: " + req;

            // 4. Отправляем ответ: identity + пустой фрейм + payload
            router.send(identity, zmq::send_flags::sndmore);
            router.send(zmq::message_t{}, zmq::send_flags::sndmore); // пустой фрейм
            router.send(zmq::buffer(reply), zmq::send_flags::none);
        }
    }

    void client(logger::Logger& logger)
    {
        std::string name = "client";
        zmq::context_t ctx(1);
        zmq::socket_t dealer(ctx, zmq::socket_type::dealer);

        dealer.set(zmq::sockopt::routing_id, name);

        dealer.connect(connString.data());
        logger.info("[{}] connected", name);

        // Отправляем 3 запроса подряд — асинхронно
        for (int i = 0; i < 3; ++i) {
            std::string req = name + " request #" + std::to_string(i);
            dealer.send(zmq::message_t{}, zmq::send_flags::sndmore); // пустой фрейм
            dealer.send(zmq::buffer(req), zmq::send_flags::none);
        }

        for (int i = 0; i < 3; ++i) {
            zmq::message_t empty;
            dealer.recv(empty, zmq::recv_flags::none); // пустой фрейм

            zmq::message_t reply;
            dealer.recv(reply, zmq::recv_flags::none);

            logger.info("[{}] got: {}", name, std::string(static_cast<char*>(reply.data()), reply.size()));
        }
    }

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };

        std::jthread s(server, std::ref(logger)),
        c(client, std::ref(logger));
    }
}

void router_dealer::experiments::TestAll()
{
    run();
}
