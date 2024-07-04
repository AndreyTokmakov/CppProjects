/**============================================================================
Name        : JRPC.cpp
Created on  : 04.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : JRPC.cpp
============================================================================**/

#include "JRPC.h"

#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace JRPC
{
    class ReverseServiceImplementation final : public StringReverse::Service
     {
        Status sendRequest(ServerContext* context,
                           const StringRequest* request,
                           StringReply* reply) override
        {
            // Obtains the original string from the request
            std::string a = request->original();

            // String reversal
            int n = a.length();
            for (int i = 0; i < n / 2; i++) std::swap(a[i], a[n - i - 1]);

            reply->set_reversed(a);
            return Status::OK;
        }
    };
}

void JRPC::TestAll()
{

}