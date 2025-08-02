/**============================================================================
Name        : Secure_Communication.cpp
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Secure_Communication.h
============================================================================**/

#include "Secure_Communication.h"
#include "DateTimeUtilities.h"

#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace Secure_Communication
{
    std::pair<std::string, std::string> generateKeys()
    {
        char public_key[41], secret_key[41];
        if (const int rc = zmq_curve_keypair(public_key, secret_key); rc != 0) {
            std::cerr << "Failed to generate keys\n";
            return {};
        }
        return {public_key, secret_key};
    }

    void server(const std::string& secretKey,
                const std::string& serverPublicKey)
    {
        zmq::context_t ctx(1);
        zmq::socket_t server(ctx, zmq::socket_type::rep);

        server.set(zmq::sockopt::curve_secretkey, secretKey);
        server.set(zmq::sockopt::curve_publickey, serverPublicKey);
        server.set(zmq::sockopt::curve_server, 1);  // признак того, что это сервер

        server.bind("tcp://*:5555");

        while (true)
        {
            zmq::message_t request;
            server.recv(request);
            std::cout << "Received: " << request.to_string() << "\n";
            server.send(zmq::buffer("World"));
        }
    }

    void client(const std::string& clientSecretKey,
                const std::string& clientPublicKey,
                const std::string& serverPublicKey)
    {
        zmq::context_t ctx(1);
        zmq::socket_t client(ctx, zmq::socket_type::req);

        std::cout << clientSecretKey.size() << std::endl;

        client.set(zmq::sockopt::curve_secretkey, clientSecretKey);
        client.set(zmq::sockopt::curve_publickey, clientPublicKey);
        client.set(zmq::sockopt::curve_serverkey, serverPublicKey);

        client.connect("tcp://localhost:5555");
        client.send(zmq::buffer("Hello1"));

        zmq::message_t reply;
        client.recv(reply);
        std::cout << "Reply: " << reply.to_string() << "\n";
    }

    void run()
    {
        const auto [serverPublicKey, serverSecretkey] = Secure_Communication::generateKeys();
        const auto [clientPublicKey, clientSecretkey] = Secure_Communication::generateKeys();

        std::vector<std::jthread> workers;
        workers.emplace_back(server, serverSecretkey, serverPublicKey);
        workers.emplace_back(client, clientSecretkey, clientPublicKey, serverPublicKey);
    }
}

void Secure_Communication::TestAll()
{
    run();
}