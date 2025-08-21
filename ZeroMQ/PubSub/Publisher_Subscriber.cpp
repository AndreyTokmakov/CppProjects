/**============================================================================
Name        : Publisher_Subscriber.cpp
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Publisher_Subscriber.h
============================================================================**/

#include "Publisher_Subscriber.hpp"
#include "DateTimeUtilities.hpp"

#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace PubSub_Simple
{
    using namespace DateTimeUtilities;
    using namespace std::string_view_literals;

    void publisher()
    {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pub);
        socket.bind("tcp://*:5555");

        int count = 0;
        while (true)
        {
            std::string msg = "Hello #" + std::to_string(count++);
            zmq::message_t message(msg.begin(), msg.end());
            socket.send(message, zmq::send_flags::none);
            std::cout << getCurrentTime() << " Published: " << msg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250u));

            if (10 == count)
            {
                msg = "QUIT";
                zmq::message_t quit(msg.begin(), msg.end());
                socket.send(quit, zmq::send_flags::none);
                break;
            }
        }

        socket.close();
    }

    void subscriber()
    {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::sub);
        socket.connect("tcp://0.0.0.0:5555");
        socket.set(zmq::sockopt::subscribe, "");

        while (true)
        {
            zmq::message_t message;
            const std::optional<size_t> result = socket.recv(message, zmq::recv_flags::none);
            if (!result) {
                std::cerr << getCurrentTime() << " Receive error\n";
                continue;
            }

            const std::string_view data(static_cast<char*>(message.data()), message.size());
            std::cout << getCurrentTime() << " Received: " << data << std::endl;

            if ("QUIT"sv == data) {
                std::cout << getCurrentTime() << " Stopping .... \n";
                break;
            }
        }

        socket.close();
    }

    void run()
    {
        std::vector<std::jthread> workers;
        workers.emplace_back(publisher);

        workers.emplace_back(subscriber);
        workers.emplace_back(subscriber);
        workers.emplace_back(subscriber);
    }
}

namespace PubSub_Topics
{
    using namespace DateTimeUtilities;
    using namespace std::string_view_literals;

    constexpr std::array<std::string_view, 2> topics { "events", "notifications" };

    void publisher()
    {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pub);
        socket.bind("tcp://0.0.0.0:5555");

        int count = 0;
        while (true)
        {
            if (10 > count)
            {
                const std::string topic { ((++count) & 1) ? topics.front() : topics.back() };
                const std::string data = "Hello #" + std::to_string(count);

                zmq::message_t topicInfoMsg(topic.begin(), topic.end());
                zmq::message_t msgData(data.begin(), data.end());

                // socket.send(message, zmq::send_flags::none);
                socket.send(topicInfoMsg, zmq::send_flags::sndmore);
                socket.send(msgData, zmq::send_flags::none);

                std::cout << getCurrentTime() << " ==> Published: '" << data << "' to '" << topic << "'\n";
                std::this_thread::sleep_for(std::chrono::milliseconds (250u));
            }
            else
            {
                constexpr std::string_view strQuit { "QUIT" };
                for (const std::string_view topic: topics)
                {
                    zmq::message_t msqQuit(strQuit.begin(), strQuit.end());
                    zmq::message_t topicInfoMsg(topic.begin(), topic.end());

                    socket.send(topicInfoMsg, zmq::send_flags::sndmore);
                    socket.send(msqQuit, zmq::send_flags::none);
                }
                break;
            }
        }

        socket.close();
    }

    void subscriber(const std::string_view name, const std::string_view topic)
    {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::sub);
        socket.connect("tcp://0.0.0.0:5555");
        socket.set(zmq::sockopt::subscribe, topic);

        while (true)
        {
            std::vector<zmq::message_t> recv_msgs;
            const std::optional<size_t> result = zmq::recv_multipart(socket, std::back_inserter(recv_msgs));
            // std::cout << result.value() << " " << recv_msgs.size() << std::endl;

            {
                const zmq::message_t& message = recv_msgs.front();
                const std::string_view data = message.to_string_view();
                std::cout << "[" << name << "] " << getCurrentTime() << " Topic: " << data << std::endl;
            }

            {
                const zmq::message_t& message = recv_msgs.back();
                const std::string_view data = message.to_string_view();
                std::cout << "[" << name << "] " << getCurrentTime()  << " Data: " << data << std::endl;

                if ("QUIT"sv == data) {
                    std::cout << "[" << name << "] " << getCurrentTime()  << " Stopping .... \n";
                    break;
                }
            }
        }

        socket.close();
    }

    void run()
    {
        std::vector<std::jthread> workers;
        workers.emplace_back(publisher);
        workers.emplace_back(subscriber, "S1", topics.front());
        workers.emplace_back(subscriber, "S2", topics.back());
        workers.emplace_back(subscriber, "S3", topics.front());
        workers.emplace_back(subscriber, "S4", topics.back());

    }

    /**
    2025-07-05 11:06:41.878291 ==> Published: Hello #1 to events
    2025-07-05 11:06:42.128510 ==> Published: Hello #2 to notifications
    2025-07-05 11:06:42.379108 ==> Published: Hello #3 to events
    2025-07-05 11:06:42.379534 Topic: events
    2025-07-05 11:06:42.379560 Data: Hello #3
    2025-07-05 11:06:42.629545 ==> Published: Hello #4 to notifications
    2025-07-05 11:06:42.880020 ==> Published: Hello #5 to events
    2025-07-05 11:06:42.880361 Topic: events
    2025-07-05 11:06:42.880406 Data: Hello #5
    2025-07-05 11:06:43.130286 ==> Published: Hello #6 to notifications
    2025-07-05 11:06:43.380478 ==> Published: Hello #7 to events
    2025-07-05 11:06:43.380905 Topic: events
    2025-07-05 11:06:43.380954 Data: Hello #7
    2025-07-05 11:06:43.630694 ==> Published: Hello #8 to notifications
    2025-07-05 11:06:43.881125 ==> Published: Hello #9 to events
    2025-07-05 11:06:43.881529 Topic: events
    2025-07-05 11:06:43.881570 Data: Hello #9
    2025-07-05 11:06:44.131641 ==> Published: Hello #10 to notifications
    2025-07-05 11:06:44.382420 Topic: events
    2025-07-05 11:06:44.382478 Data: QUIT
    2025-07-05 11:06:44.382487 Stopping ....
    **/
}



void Publisher_Subscriber::TestAll()
{
    // PubSub_Simple::run();
    PubSub_Topics::run();
}