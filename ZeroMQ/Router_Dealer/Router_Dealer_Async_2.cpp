/**============================================================================
Name        : RouterDealerAsynch.cpp
Created on  : 05.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RouterDealerAsynch.cpp
============================================================================**/

#include <set>
#include <deque>

#include "Router_Dealer.hpp"
#include "Logger.hpp"

#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace
{
    using namespace std::string_view_literals;
    constexpr std::string_view bindAddr    { "tcp://*:5556" };
    constexpr std::string_view connectAddr { "tcp://127.0.0.1:5556" };

    struct PendingRequest
    {
        std::string identity;
        std::string payload;
        std::chrono::steady_clock::time_point ready_at;
    };

    void server(logger::Logger& logger)
    {
        zmq::context_t ctx(1);
        zmq::socket_t router(ctx, zmq::socket_type::router);

        router.bind(std::string(bindAddr));

        logger.info("async ROUTER bound on {}", bindAddr);

        std::deque<PendingRequest> pending;
        int tick = 0;

        while (true)
        {
            zmq::pollitem_t items[] = {
                { .socket = router.handle(), .fd = 0, .events = ZMQ_POLLIN, .revents = 0 }
            };

            const int rc = zmq::poll(items, 1, std::chrono::milliseconds(100));
            if (rc > 0 && (items[0].revents & ZMQ_POLLIN))
            {
                // Read the whole multipart frame at once.
                std::vector<zmq::message_t> frames;
                std::optional<size_t> received = zmq::recv_multipart(router, std::back_inserter(frames));

                if (!received.has_value() || frames.size() < 3)
                {
                    logger.warn("server: unexpected frame count: {}", frames.size());
                    continue;
                }

                // frames[0] = identity, frames[1] = empty delimiter, frames[2] = payload.
                const zmq::message_t& identity = frames[0];
                const zmq::message_t& payload  = frames[2];

                std::string id (static_cast<const char*>(identity.data()), identity.size());
                std::string req(static_cast<const char*>(payload.data()),  payload.size());

                logger.info("server: queued from {}: {}", id, req);

                PendingRequest pendingRequest {
                    .identity = std::move(id),
                    .payload  = std::move(req),
                    .ready_at = std::chrono::steady_clock::now() + std::chrono::milliseconds(300)
                };
                pending.push_back(std::move(pendingRequest));
            }

            // Send everything whose processing delay has elapsed.
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            while (!pending.empty() && pending.front().ready_at <= now)
            {
                PendingRequest& p = pending.front();
                std::string reply = "echo: " + p.payload;

                std::vector<zmq::message_t> out;
                out.emplace_back(p.identity.data(), p.identity.size());
                out.emplace_back();                              // empty delimiter
                out.emplace_back(reply.data(), reply.size());

                zmq::send_multipart(router, out);

                logger.info("server: replied to {} -> {}", p.identity, reply);
                pending.pop_front();
            }

            ++tick;
            if (tick % 10 == 0)
            {
                logger.info("server: tick {}, pending={}", tick, pending.size());
            }
        }

        logger.info("server: stopped");
    }

    void client(const std::string& name, logger::Logger& logger)
    {
        zmq::context_t ctx(1);
        zmq::socket_t dealer(ctx, zmq::socket_type::dealer);
        dealer.set(zmq::sockopt::routing_id, name);
        dealer.connect(std::string(connectAddr));

        logger.info("client {}: async DEALER connected to {}", name, connectAddr);

        std::set<std::string> in_flight;
        int next_id = 0;
        int tick    = 0;
        int sent_total = 0;
        int recv_total = 0;

        while (true)
        {
            // Send a new request roughly every 500 ms.
            if (tick % 5 == 0)
            {
                std::string req = name + " req#" + std::to_string(next_id);

                std::vector<zmq::message_t> out;
                out.emplace_back();                              // empty delimiter
                out.emplace_back(req.data(), req.size());
                zmq::send_multipart(dealer, out);

                in_flight.insert(req);
                ++sent_total;
                logger.info("client {}: sent {} (in flight: {})",
                            name, req, in_flight.size());
                ++next_id;
            }

            zmq::pollitem_t items[] = {
                { .socket = dealer.handle(), .fd = 0, .events = ZMQ_POLLIN, .revents = 0 }
            };

            const int rc = zmq::poll(items, 1, std::chrono::milliseconds(100));
            if (rc > 0 && (items[0].revents & ZMQ_POLLIN))
            {
                std::vector<zmq::message_t> frames;
                std::optional<size_t> received = zmq::recv_multipart(dealer, std::back_inserter(frames));
                if (!received.has_value() || frames.empty())
                {
                    logger.warn("client {}: empty reply", name);
                    continue;
                }

                // DEALER receives replies as [""][payload].
                const zmq::message_t& reply = frames.back();
                std::string r(static_cast<const char*>(reply.data()), reply.size());
                ++recv_total;
                logger.info("client {}: got {}", name, r);

                for (std::set<std::string>::iterator it = in_flight.begin(); it != in_flight.end(); ++it)
                {
                    if (r.find(*it) != std::string::npos)
                    {
                        logger.info("client {}: matched response for {}", name, *it);
                        in_flight.erase(it);
                        break;
                    }
                }
            }

            ++tick;
            if (tick % 10 == 0) {
                logger.info("client {}: tick {}, sent={}, recv={}, in flight={}",
                    name, tick, sent_total, recv_total, in_flight.size());
            }
        }

        logger.info("client {}: stopped", name);
    }

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };
        std::vector<std::jthread> tasks;

        tasks.emplace_back(server, std::ref(logger));

        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
        tasks.emplace_back(client, "Client-1", std::ref(logger));
    }
}

void router_dealer::asynch_2::TestAll()
{
    run();
}
