/**============================================================================
Name        : RouterDealerAsynch.cpp
Created on  : 05.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RouterDealerAsynch.cpp
============================================================================**/

#include "Router_Dealer.hpp"
#include "Logger.hpp"

#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <deque>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
    using namespace std::string_view_literals;
    constexpr std::string_view bindAddr    { "tcp://*:5556" };
    constexpr std::string_view connectAddr { "tcp://127.0.0.1:5556" };

    struct PendingRequest
    {
        std::string identity;   // routing id of the sender
        std::string payload;    // request body
        std::chrono::steady_clock::time_point ready_at;
    };

    class AsyncRouterServer
    {
    public:
        AsyncRouterServer(logger::Logger& logger, std::string  bind_address):
            logger { logger },
            bindAddress { std::move(bind_address) },
            zmqContext { 1 },
            zmqSocket { zmqContext, zmq::socket_type::router }
        {
            frames.reserve(8);
            outFrames.reserve(8);
            replyBuffer.reserve(256);

            pollItems.front() = {
                .socket  = zmqSocket.handle(),
                .fd      = 0,
                .events  = ZMQ_POLLIN,
                .revents = 0
            };

            // Reject sends to unknown identities instead of silently dropping.
            zmqSocket.set(zmq::sockopt::router_mandatory, 1);
            // Replace an old session if a peer reconnects with the same id.
            zmqSocket.set(zmq::sockopt::router_handover, 1);

            zmqSocket.bind(bindAddress);

            logger.info("server: async ROUTER bound on {}", bindAddress);
        }

        AsyncRouterServer(const AsyncRouterServer&)            = delete;
        AsyncRouterServer& operator=(const AsyncRouterServer&) = delete;

        void run()
        {
            running.store(true, std::memory_order_relaxed);
            while (running.load(std::memory_order_relaxed))
            {
                pollAndReceive();
                flushReadyReplies();
            }
            logger.info("server: stopped");
        }

        void stop() noexcept {
            running.store(false, std::memory_order_relaxed);
        }

    private:
        constexpr static std::chrono::milliseconds processingDelay = std::chrono::milliseconds(300);
        constexpr static std::chrono::milliseconds pollTimeout     = std::chrono::milliseconds(100);

        // [3] Hard cap on the pending queue to bound memory usage.
        constexpr static std::size_t maxPending = 4096;

        void pollAndReceive()
        {
            pollItems[0].revents = 0;

            const int rc = zmq::poll(pollItems.data(), 1, pollTimeout);
            if (rc <= 0 || (pollItems[0].revents & ZMQ_POLLIN) == 0) {
                return;
            }

            // Drain all available messages in one poll wake-up.
            while (true)
            {
                frames.clear();
                std::optional<std::size_t> received = zmq::recv_multipart(zmqSocket, std::back_inserter(frames), zmq::recv_flags::dontwait);
                if (!received.has_value() || frames.empty()) {
                    return;
                }

                if (frames.size() < 3) {
                    logger.warn("server: unexpected frame count: {}", frames.size());
                    continue;
                }

                // frames[0] = identity, frames[1] = empty delimiter,
                // frames[2] = payload. Copy only the two we keep.
                const zmq::message_t& identity = frames[0];
                const zmq::message_t& payload  = frames[2];

                PendingRequest pr;
                pr.identity.assign(static_cast<const char*>(identity.data()), identity.size());
                pr.payload .assign(static_cast<const char*>(payload.data()),  payload.size());
                pr.ready_at = std::chrono::steady_clock::now() + processingDelay;

                // [3] Drop the request if the queue is full.
                if (pending.size() >= maxPending)
                {
                    logger.warn("server: pending queue full ({}), dropping request from {}", pending.size(), pr.identity);
                    continue;
                }

                // Log only the identity in the hot path, not the payload.
                logger.info("server: queued from {}", pr.identity);
                pending.push_back(std::move(pr));
            }
        }

        void flushReadyReplies()
        {
            const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

            // Call front() once per iteration.
            while (!pending.empty())
            {
                PendingRequest& p = pending.front();
                if (p.ready_at > now) {
                    break;
                }

                // Build reply in a reusable buffer to avoid allocations.
                replyBuffer.clear();
                replyBuffer.append("echo: ");
                replyBuffer.append(p.payload);

                outFrames.clear();
                outFrames.emplace_back(p.identity.data(), p.identity.size());
                outFrames.emplace_back(); // empty delimiter
                outFrames.emplace_back(replyBuffer.data(), replyBuffer.size());

                // Handle send failures without killing the worker thread.
                try {
                    zmq::send_multipart(zmqSocket, outFrames);
                    logger.info("server: replied to {}", p.identity);
                } catch (const zmq::error_t& e) {
                    logger.error("server: send failed for {}: {}", p.identity, e.what());
                }

                pending.pop_front();
            }
        }

        logger::Logger& logger;
        std::string bindAddress;

        zmq::context_t zmqContext;
        zmq::socket_t  zmqSocket;

        // Poll item configured once in the constructor.
        std::array<zmq::pollitem_t, 1> pollItems {};

        // Reused across iterations to avoid reallocations.
        std::vector<zmq::message_t> frames;
        std::vector<zmq::message_t> outFrames;
        std::string                 replyBuffer;

        std::deque<PendingRequest> pending;

        //  Starts true; run() exits immediately if stop() was called first.
        std::atomic_bool running { true };
    };

    class AsyncDealerClient
    {
    public:
        AsyncDealerClient(logger::Logger& logger,
                          std::string clientName,
                          std::string connect_address):
            logger { logger },
            name { std::move(clientName) },
            connectAddress { std::move(connect_address) },
            zmqContext { 1 },
            zmqSocket(zmqContext, zmq::socket_type::dealer)
        {
            frames.reserve(8);
            outFrames.reserve(8);
            pollItems.front() = {
                .socket  = zmqSocket.handle(),
                .fd      = 0,
                .events  = ZMQ_POLLIN,
                .revents = 0
            };

            // Set the routing id via the C API to stay compatible with all libzmq versions that expose ZMQ_ROUTING_ID (aliased to
            // ZMQ_IDENTITY on older builds). cppzmq's typed setter for routing_id is not available everywhere and can throw EINVAL.
            const int rc = zmq_setsockopt(zmqSocket.handle(), ZMQ_ROUTING_ID, name.data(), name.size());
            if (rc != 0) {
                throw zmq::error_t();
            }

            zmqSocket.connect(connectAddress);

            logger.info("client {}: async DEALER connected to {}", this->name, connectAddress);
        }

        AsyncDealerClient(const AsyncDealerClient&)            = delete;
        AsyncDealerClient& operator=(const AsyncDealerClient&) = delete;

        void run()
        {
            std::chrono::steady_clock::time_point nextSend = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point nextHeartbeat = std::chrono::steady_clock::now() + heartbeatInterval;

            while (running.load(std::memory_order_relaxed))
            {
                const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                if (now >= nextSend) {
                    sendRequest();
                    nextSend = now + sendInterval;
                }

                pollAndReceive();
                if (now >= nextHeartbeat) {
                    logger.info("client {}: alive, sent={}, recv={}, in flight={}",name, sentTotal, recvTotal, inFlight.size());
                    nextHeartbeat = now + heartbeatInterval;
                }
            }

            logger.info("client {}: stopped", name);
        }

        void stop() noexcept {
            running.store(false, std::memory_order_relaxed);
        }

    private:
        constexpr static std::chrono::milliseconds pollTimeout  = std::chrono::milliseconds(100);
        constexpr static std::chrono::milliseconds sendInterval = std::chrono::milliseconds(500);
        constexpr static std::chrono::milliseconds heartbeatInterval = std::chrono::seconds(1'000);

        void sendRequest()
        {
            std::string req = name + " req#" + std::to_string(nextId);

            outFrames.clear();
            outFrames.emplace_back();                          // empty delimiter
            outFrames.emplace_back(req.data(), req.size());

            try {
                zmq::send_multipart(zmqSocket, outFrames);
                inFlight.insert(req);
                ++sentTotal;
                ++nextId;
                logger.info("client {}: sent {} (in flight: {})", name, req, inFlight.size());
            } catch (const zmq::error_t& e) {
                logger.error("client {}: send failed: {}", name, e.what());
            }
        }

        void pollAndReceive()
        {
            pollItems[0].revents = 0;

            const int rc = zmq::poll(pollItems.data(), 1, pollTimeout);
            if (rc <= 0 || (pollItems[0].revents & ZMQ_POLLIN) == 0) {
                return;
            }

            while (true)
            {
                frames.clear();
                std::optional<std::size_t> received = zmq::recv_multipart(zmqSocket, std::back_inserter(frames), zmq::recv_flags::dontwait);
                if (!received.has_value() || frames.empty()) {
                    return;
                }

                // DEALER receives replies as [""][payload].
                const zmq::message_t& payload = frames.back();
                std::string reply(static_cast<const char*>(payload.data()), payload.size());
                ++recvTotal;

                logger.info("client {}: got {}", name, reply);
                for (std::set<std::string>::iterator it = inFlight.begin(); it != inFlight.end(); ++it) {
                    if (reply.find(*it) != std::string::npos) {
                        logger.info("client {}: matched response for {}", name, *it);
                        inFlight.erase(it);
                        break;
                    }
                }
            }
        }

        logger::Logger& logger;
        std::string     name;
        std::string     connectAddress;

        zmq::context_t  zmqContext;
        zmq::socket_t   zmqSocket;

        std::array<zmq::pollitem_t, 1> pollItems {};

        // Reused across iterations to avoid reallocations.
        std::vector<zmq::message_t> frames;
        std::vector<zmq::message_t> outFrames;

        std::set<std::string> inFlight;

        std::atomic_bool running { true };
        int              nextId    { 0 };
        int              sentTotal { 0 };
        int              recvTotal { 0 };
    };

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };
        try {
            AsyncRouterServer server { logger, std::string(bindAddr) };
            AsyncDealerClient client { logger, std::string("Client-1"), std::string(connectAddr) };

            std::vector<std::jthread> tasks;

            tasks.emplace_back([&server] {
                server.run();
            });

            std::this_thread::sleep_for(std::chrono::milliseconds(500U));

            tasks.emplace_back([&client] {
                client.run();
            });

            std::this_thread::sleep_for(std::chrono::seconds(5));

            client.stop();
            server.stop();
        }
        catch (const zmq::error_t& e) {
            logger.error("zmq error: {} (errno={})", e.what(), e.num());
        }
        catch (const std::exception& e) {
            logger.error("error: {}", e.what());
        }
    }
}

void router_dealer::asynch_server_class::TestAll()
{
    run();
}

