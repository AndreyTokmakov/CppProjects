/**============================================================================
Name        : Experiments.cpp
Created on  : 27.10.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments
============================================================================**/

#include "TestWebServer.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

namespace TestWebServer
{
    /** WebServer class declaration: **/
    class WebServer {
    private:
        using Socket = int;

        /** Server listener socket: **/
        Socket serverSocket { INVALID_SOCKET };

        /** Clients table: **/
        std::vector<Socket> clients;

        /** Receive buffer: **/
        char clientBuffer[1536] {}; // TODO: --> std::array

        /** SELECT timeout: **/
        timeval timeout {};            // TODO: --> std::chrono

        /** **/
        int bytesRead { 1 };

        /** Request: **/
        std::string request;

        /** Response: **/
        std::string response;

    protected:

        [[maybe_unused]]
        bool CreateListenerSocket() {
            serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == serverSocket) {
                std::cout << "Failed to create the TCP socket. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        [[nodiscard]]
        bool SetListenerOptions() const {
            if (SOCKET_ERROR == fcntl(serverSocket, F_SETFL, O_NONBLOCK)) {
                std::cout << "FCNTL failed. Error = " << errno << std::endl;
                return false;
            }

            if (int yes{1}; SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                std::cout << "Setsockopt failed. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        [[nodiscard]]
        bool Bind(std::string_view host, unsigned short port) const {
            sockaddr_in server { PF_INET, htons(port),{.s_addr = inet_addr(host.data())}, {}};
            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
                std::cout << "Failed to bind socket. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        [[nodiscard]]
        bool Listen(int backlog = 10) const {
            if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
                std::cout << "Failed to Listen the socket. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

    private:
        bool Wait4Event(fd_set *readset) {
            const int maxClient = clients.empty() ? 0 : *max_element(clients.cbegin(), clients.cend());
            const int mx = std::max(serverSocket, maxClient);

            if (SOCKET_ERROR == ::select(mx + 1, readset, nullptr, nullptr, &timeout)) {
                std::cout << "Select failed. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        bool HandleConnectionEvent(const fd_set *readset) {
            if (FD_ISSET(this->serverSocket, readset)) {
                Socket clientSocket = ::accept(serverSocket, nullptr, nullptr);
                if (INVALID_SOCKET == clientSocket) {
                    std::cout << "Failed to accept client connection. Error = " << errno << std::endl;
                    return false;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                clients.push_back(clientSocket);
                std::cout << "Client connection accepted (Clients in table " << clients.size() << ")" << std::endl;
            }
            return true;
        }

        /** **/
        bool HandleReceiveDataEvent(const fd_set *readset)
        {
            for (auto iterClient = clients.begin(); iterClient != clients.end(); )
            {
                Socket clientSocket = *iterClient;

                std::cout << "HandleReceiveDataEvent(" << clientSocket << "):" << __LINE__
                          << ". Size = " << clients.size() << std::endl;
                if (clients.empty()) {
                    std::cout << "END! Break" << std::endl;
                    break;
                }


                if (FD_ISSET(clientSocket, readset)) { /** Some data received. Reading it.... **/
                    std::cout << "HandleReceiveDataEvent(" << clientSocket << "):"  << __LINE__ << std::endl;

                    bytesRead = ::recv(clientSocket, clientBuffer, sizeof(clientBuffer), 0);
                    if (bytesRead <= 0) { /** Connection closed. Removing socket from the set. **/
                        ::close(clientSocket);
                        clients.erase(iterClient++);
                        std::cout << "Connection closed (Clients in table " << clients.size() << ")" << std::endl;
                        continue;
                    }
                    /*** Handle client request: ***/
                    request.assign(clientBuffer, bytesRead);
                    std::cout << request;

                    /*** Send response: ***/
                    ::send(clientSocket, const_cast<char *>(response.c_str()), response.length(), 0);
                }

                ++iterClient;
            }
            return true;
        }

    public:
        WebServer()
        {
            // FIXME
            timeout.tv_sec = 15;
            timeout.tv_usec = 0;

            std::string pageHTML = "<html><head><title>AndTokmServer</title></head><body BGCOLOR='grey'>Welcome</body></html>";
            response = "HTTP/1.1 200 OK\r\n";
            response += "Date: Wed, 11 Feb 2009 11:20:59 GMT\r\n";
            response += "Server: AndTokmServer\r\n";
            response += "X-Powered-By: PHP/5.2.4-2ubuntu5wm1\r\n";
            response += "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\r\n";
            response += "Content-Language: ru\r\n";
            response += "Content-Type: text/html; charset=utf-8\r\n";
            response += "Content-Length: " + std::to_string(pageHTML.length()) + "\r\n";
            response += "Connection: close\r\n";
            response += "\r\n" + pageHTML + "\r\n";
        }

        ~WebServer()
        {
            if (INVALID_SOCKET != serverSocket)
                ::close(serverSocket);
        }

        bool InitServer(std::string_view host,
                        unsigned short port = 8080)
        {
            if (!CreateListenerSocket())
                return false;
            if (!SetListenerOptions())
                return false;
            if (!Bind(host, port))
                return false;
            if (!Listen())
                return false;
            return true;
        }

        bool Run()
        {
            while (true) {
                fd_set readset;
                FD_ZERO(&readset);
                FD_SET (serverSocket, &readset);

                for (auto& client: clients)
                    FD_SET(client, &readset);
                if (!Wait4Event(&readset))
                    break;
                HandleConnectionEvent(&readset);
                HandleReceiveDataEvent(&readset);
            }
            return true;
        }
    };

};

void TestWebServer::Tests()
{
    WebServer W;
    if (W.InitServer("0.0.0.0", 52525)) {
        W.Run();
    }
};
