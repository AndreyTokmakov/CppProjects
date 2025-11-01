/**============================================================================
Name        : CurlCpp.cpp
Created on  : 01.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CurlCpp.cpp
============================================================================**/

#include <sstream>
#include <iostream>

#include "CurlCpp.hpp"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

void simple_example()
{
    try
    {
        curlpp::Easy request;
        std::ostringstream response;
        request.setOpt(new curlpp::options::Url("https://httpbin.org/get"));
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();
        std::cout << "Response:\n" << response.str() << std::endl;
    }
    catch (curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }
}

void CurpCpp::TestAll()
{
    simple_example();
}
