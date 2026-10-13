/**============================================================================
Name        : AsyncClients.hpp
Created on  : 13.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsyncClients.hpp
============================================================================**/

#ifndef CPPPROJECTS_ASYNCCLIENTS_HPP
#define CPPPROJECTS_ASYNCCLIENTS_HPP

#include <string_view>
#include <vector>

namespace async_zmq_clients
{
    void testAll(const std::vector<std::string_view>& args);
}

#endif //CPPPROJECTS_ASYNCCLIENTS_HPP
