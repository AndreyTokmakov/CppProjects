/**============================================================================
Name        : Router_Dealer_Poller_Timeout.cpp
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer_Poller_Timeout.cpp
============================================================================**/

#include "Router_Dealer_Poller_Timeout.hpp"

#include <thread>
#include <format>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace Router_Dealer_Poller_Timeout
{

    constexpr uint16_t clientPort { 5555 };
    constexpr uint16_t serverPort { 5556 };

    void broker()
    {
        zmq::context_t ctx(1);

        zmq::socket_t frontend(ctx, zmq::socket_type::router);
        zmq::socket_t backend(ctx, zmq::socket_type::router);

        frontend.bind(std::format("tcp://*:{}", clientPort));
        backend.bind(std::format("tcp://*:{}", serverPort));


        /*
        zmq::poller_t<> poller;
        poller.add(frontend, zmq::event_flags::pollin);
        poller.add(backend, zmq::event_flags::pollin);

        std::deque<zmq::message_t> workers;

        while (true)
        {
            std::vector<zmq::poller_event<>> events;
            poller.wait_all(events, std::chrono::milliseconds(1000));

            for (auto& event : events) {
                if (event.socket == backend) {
                    zmq::multipart_t msg;
                    msg.recv(backend);

                    // ID воркера (msg[0])
                    workers.push_back(msg.at(0).clone());

                    // если это задача, пересылаем на клиента
                    if (msg.size() > 2) {
                        zmq::multipart_t task;
                        task.push_back(std::move(msg.at(2))); // client id
                        task.push_back(zmq::message_t());     // empty frame
                        task.push_back(std::move(msg.at(3))); // payload
                        task.send(frontend);
                    }

                } else if (event.socket == frontend && !workers.empty()) {
                    zmq::multipart_t msg;
                    msg.recv(frontend);

                    zmq::message_t worker_id = workers.front();
                    workers.pop_front();

                    zmq::multipart_t fwd;
                    fwd.add(std::move(worker_id));
                    fwd.add(zmq::message_t());
                    fwd.add(std::move(msg.at(0))); // client id
                    fwd.add(zmq::message_t());
                    fwd.add(std::move(msg.at(2))); // payload
                    fwd.send(backend);
                }
            }
        }
        */
    }
}


void Router_Dealer_Poller_Timeout::TestAll()
{

}
