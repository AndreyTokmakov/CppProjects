========================================================================================================================
            Links
========================================================================================================================

# zmq_setsockopt

        https://libzmq.readthedocs.io/en/latest/zmq_setsockopt.html


========================================================================================================================
0. Установка зависимостей
========================================================================================================================

sudo apt update
sudo apt install libzmq3-dev
sudo apt install pkg-config


# cppzmq можно скачать вручную
git clone https://github.com/zeromq/cppzmq.git
# и добавить include путь в CMake или копировать zmq.hpp в include/



cmake -DCMAKE_BUILD_TYPE=Release -B./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPPZMQ_BUILD_TESTS=OFF -B./build




========================================================================================================================
1. CMakeLists.txt
========================================================================================================================

cmake_minimum_required(VERSION 3.12)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Найдём libzmq (C-заголовки + библиотека)
pkg_check_modules(PC_ZMQ libzmq REQUIRED)
find_path(ZMQ_INCLUDE_DIR zmq.h PATHS ${PC_ZMQ_INCLUDE_DIRS})
find_library(ZMQ_LIBRARY NAMES zmq PATHS ${PC_ZMQ_LIBRARY_DIRS})

# Найдём cppzmq (C++-обёртка)
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/publisher.cpp
    src/subscriber.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${ZMQ_INCLUDE_DIR}
        ${CPPZMQ_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${ZMQ_LIBRARY}
        Threads::Threads
)


========================================================================================================================
                Server
========================================================================================================================

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

void run_publisher() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::pub);
    socket.bind("tcp://*:5555");

    int count = 0;
    while (true) {
        std::string msg = "Hello #" + std::to_string(count++);
        zmq::message_t message(msg.begin(), msg.end());
        socket.send(message, zmq::send_flags::none);
        std::cout << "Published: " << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


========================================================================================================================
                Client  - src/subscriber.cpp
========================================================================================================================


#include <zmq.hpp>
#include <string>
#include <iostream>

void run_subscriber() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::sub);
    socket.connect("tcp://localhost:5555");
    socket.set(zmq::sockopt::subscribe, "");

    while (true) {
        zmq::message_t message;
        socket.recv(message, zmq::recv_flags::none);
        std::string data(static_cast<char*>(message.data()), message.size());
        std::cout << "Received: " << data << std::endl;
    }
}

========================================================================================================================
               Demo
========================================================================================================================


#include <iostream>
#include <thread>

extern void run_publisher();
extern void run_subscriber();

int main() {
    std::thread pub(run_publisher);
    std::thread sub(run_subscriber);

    pub.join();
    sub.join();

    return 0;
}

=========================================================================================================================
             ROUTER-DEALER с несколькими клиентами и воркерами
=========================================================================================================================

#include <zmq.hpp>
#include <iostream>

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t frontend(ctx, zmq::socket_type::router);
    zmq::socket_t backend(ctx, zmq::socket_type::dealer);

    frontend.bind("tcp://*:5555");  // Clients
    backend.bind("tcp://*:5556");   // Workers

    zmq::pollitem_t items[] = {
        { static_cast<void*>(frontend), 0, ZMQ_POLLIN, 0 },
        { static_cast<void*>(backend),  0, ZMQ_POLLIN, 0 }
    };

    while (true) {
        zmq::poll(&items[0], 2, -1);

        // From client to worker
        if (items[0].revents & ZMQ_POLLIN) {
            std::vector<zmq::message_t> parts;
            zmq::message_t msg;
            while (true) {
                frontend.recv(msg, zmq::recv_flags::none);
                bool more = msg.more();
                parts.push_back(std::move(msg));
                if (!more) break;
            }

            for (size_t i = 0; i < parts.size(); ++i) {
                backend.send(parts[i], i + 1 < parts.size() ? zmq::send_flags::sndmore : zmq::send_flags::none);
            }
        }

        // From worker to client
        if (items[1].revents & ZMQ_POLLIN) {
            std::vector<zmq::message_t> parts;
            zmq::message_t msg;
            while (true) {
                backend.recv(msg, zmq::recv_flags::none);
                bool more = msg.more();
                parts.push_back(std::move(msg));
                if (!more) break;
            }

            for (size_t i = 0; i < parts.size(); ++i) {
                frontend.send(parts[i], i + 1 < parts.size() ? zmq::send_flags::sndmore : zmq::send_flags::none);
            }
        }
    }
}


#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, zmq::socket_type::dealer);
    sock.connect("tcp://localhost:5556");

    while (true) {
        zmq::message_t client_id;
        zmq::message_t empty;
        zmq::message_t request;

        sock.recv(client_id, zmq::recv_flags::none);
        sock.recv(empty, zmq::recv_flags::none);
        sock.recv(request, zmq::recv_flags::none);

        std::string msg = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << "[Worker] Received: " << msg << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::string reply = "Reply to: " + msg;
        sock.send(client_id, zmq::send_flags::sndmore);
        sock.send(zmq::message_t(), zmq::send_flags::sndmore);
        sock.send(zmq::buffer(reply), zmq::send_flags::none);
    }
}

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.connect("tcp://localhost:5555");

    for (int i = 0; i < 5; ++i) {
        std::string msg = "request " + std::to_string(i);
        sock.send(zmq::buffer(msg), zmq::send_flags::none);

        zmq::message_t reply;
        sock.recv(reply, zmq::recv_flags::none);
        std::string resp = std::string(static_cast<char*>(reply.data()), reply.size());
        std::cout << "[Client] Got reply: " << resp << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}