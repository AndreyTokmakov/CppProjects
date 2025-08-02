/**============================================================================
Name        : Experiments.cpp
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments.cpp
============================================================================**/

#include "Experiments.hpp"

#include <iostream>
#include <print>
#include <zmq_addon.hpp>

namespace Experiments::Other
{
    void checkVersion()
    {
        int major { 0 }, minor { 0 }, patch { 0 };
        zmq::version(&major, &minor, &patch);
        std::println("ZeroMQ version: {}.{}.{}" , major, minor , patch);
    }
}

namespace Experiments::Multipart_Messages
{
    using namespace std::string_view_literals;

    void demo()
    {
        zmq::context_t ctx;
        zmq::socket_t sock1(ctx, zmq::socket_type::push);
        zmq::socket_t sock2(ctx, zmq::socket_type::pull);
        sock1.bind("tcp://127.0.0.1:*");

        const std::string last_endpoint = sock1.get(zmq::sockopt::last_endpoint);
        std::cout << "Connecting to " << last_endpoint << std::endl;
        sock2.connect(last_endpoint);

        std::array<zmq::const_buffer, 2> send_msgs = {
                zmq::str_buffer("foo"),
                zmq::str_buffer("bar!")
        };

        if (!zmq::send_multipart(sock1, send_msgs))
            return;

        std::vector<zmq::message_t> messages;
        const std::optional<uint64_t> ret = zmq::recv_multipart(sock2, std::back_inserter(messages));
        if (!ret)
            return;
        std::cout << "Got " << *ret<< " messages. size = " << messages.size() << std::endl;

        for (const auto& msg: messages) {
            std::cout << msg.to_string_view() << std::endl;
        }

        // Connecting to tcp://127.0.0.1:42265
        // Got 2 messages. size = 2
        // foo
        // bar!
    }

    void demo_2()
    {
        zmq::context_t ctx;
        zmq::socket_t sock1(ctx, zmq::socket_type::push);
        zmq::socket_t sock2(ctx, zmq::socket_type::pull);
        sock1.bind("tcp://127.0.0.1:*");

        const std::string last_endpoint = sock1.get(zmq::sockopt::last_endpoint);
        sock2.connect(last_endpoint);

        constexpr std::array<std::string_view, 5> data {
            "one"sv, "two"sv, "three"sv, "four"sv, "five"sv
        };

        std::ranges::for_each_n(data.begin(), data.size() - 1, [&] (const std::string_view& str) {
            sock1.send(zmq::message_t {str}, zmq::send_flags::sndmore);
        });
        sock1.send(zmq::message_t {data.back()}, zmq::send_flags::none);

        std::vector<zmq::message_t> messages;
        const std::optional<uint64_t> ret = zmq::recv_multipart(sock2, std::back_inserter(messages));
        if (!ret)
            return;

        for (const auto& msg: messages) {
            std::cout << msg.to_string_view() << std::endl;
        }
    }

    void demo_3()
    {
        zmq::context_t ctx;
        zmq::socket_t sock1(ctx, zmq::socket_type::push);
        zmq::socket_t sock2(ctx, zmq::socket_type::pull);
        sock1.bind("tcp://127.0.0.1:*");

        const std::string last_endpoint = sock1.get(zmq::sockopt::last_endpoint);
        sock2.connect(last_endpoint);

        zmq::multipart_t msgs;
        constexpr std::array<std::string_view, 5> data {
            "one"sv, "two"sv, "three"sv, "four"sv, "five"sv
        };
        for (const std::string_view& str: data) {
            msgs.append(zmq::message_t {str});
        }

        if (!zmq::send_multipart(sock1, msgs))
            return;

        std::vector<zmq::message_t> messages;
        const std::optional<uint64_t> ret = zmq::recv_multipart(sock2, std::back_inserter(messages));
        if (!ret)
            return;

        for (const auto& msg: messages) {
            std::cout << msg.to_string_view() << std::endl;
        }

        // one
        // two
        // three
        // four
        // five
    }
}

namespace Experiments::Buffers
{

    void tests()
    {
        std::string data { "Test_Message" };
        std::string constData { "Test_Message" };

        // Basic construction
        zmq::mutable_buffer mbuf = zmq::buffer(data.data(), data.size());
        zmq::const_buffer cbuf = zmq::buffer(constData.data(), constData.size());
    }

}

void Experiments::TestAll()
{
    // Multipart_Messages::demo();
    // Multipart_Messages::demo_2();
    // Multipart_Messages::demo_3();

    Other::checkVersion();

}