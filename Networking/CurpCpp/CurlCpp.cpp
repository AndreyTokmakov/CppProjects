/**============================================================================
Name        : CurlCpp.cpp
Created on  : 01.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CurlCpp.cpp
============================================================================**/

#include <iostream>
#include <fstream>
#include <sstream>
#include <print>

#include "CurlCpp.hpp"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

void simple_example()
{
    try
    {
        curlpp::Cleanup cleanup;
        curlpp::Easy request;
        std::ostringstream response;
        request.setOpt(new curlpp::options::Url("https://httpbin.org/get"));
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();
        std::cout << "Response:\n" << response.str() << std::endl;
    }
    catch (curlpp::RuntimeError & e) {
        std::cerr << e.what() << std::endl;
    }
    catch (curlpp::LogicError & e){
        std::cerr << e.what() << std::endl;
    }
}


namespace verbose_callback
{
    using namespace curlpp::Options;
    using namespace std::placeholders;

    struct Debugger
    {
        size_t write(curl_infotype, const char *data, const size_t size)
        {
#if 0
            fprintf(stderr, "Debug: ");
            fwrite(data, size, 1, stderr);
#else

            std::cerr.write(data, size);
#endif
            return size;
        }
    };

    void run()
    {
        try
        {
            curlpp::Cleanup cleanup;
            curlpp::Easy request;

            Debugger obj;

            std::ostringstream response;
            request.setOpt(new curlpp::options::Url("https://httpbin.org/get"));
            request.setOpt(Verbose(true));
            request.setOpt(new curlpp::options::WriteStream(&response));
            request.setOpt(DebugFunction(std::bind(&Debugger::write, &obj, _1, _2, _3)));

            request.perform();
            std::cout << "Response:\n" << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace get_info
{
    using namespace curlpp::Options;
    using namespace std::placeholders;

    void run()
    {
        try
        {
            curlpp::Cleanup cleanup;
            curlpp::Easy request;

            std::ostringstream response;
            request.setOpt(new curlpp::options::Url("https://httpbin.org/get"));
            request.setOpt(Verbose(true));
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();
            std::cout << "Response:\n" << response.str() << std::endl;

            std::string effURL;
            curlpp::infos::EffectiveUrl::get(request, effURL);
            std::cout << "Effective URL: " << effURL << std::endl;

            // other way to retreive URL
            std::cout << std::endl << "Effective URL: " << curlpp::infos::EffectiveUrl::get(request) << std::endl;
            std::cout << "Response code: " << curlpp::infos::ResponseCode::get(request) << std::endl;
            std::cout << "SSL engines: " << curlpp::infos::SslEngines::get(request) << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace post_JSON_request
{
    void run()
    {
        try {
            curlpp::Cleanup cleanup;
            curlpp::Easy request;

            const std::string data = R"({"name": "AndTokm", "role": "Developer"})";

            std::ostringstream response;
            const std::list<std::string> headers = {
                "Content-Type: application/json"
            };

            request.setOpt(new curlpp::options::Url("https://httpbin.org/post"));
            request.setOpt(new curlpp::options::HttpHeader(headers));
            request.setOpt(new curlpp::options::PostFields(data));
            request.setOpt(new curlpp::options::PostFieldSize(data.size()));
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();
            std::cout << "Response:\n" << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace multipart_upload
{
    void run()
    {
        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            std::ostringstream response;

            curlpp::Forms formParts;
            formParts.push_back(new curlpp::FormParts::Content("field1", "value1"));
            formParts.push_back(new curlpp::FormParts::File("file", "example.txt"));

            request.setOpt(new curlpp::options::Url("https://httpbin.org/post"));
            request.setOpt(new curlpp::options::HttpPost(formParts));
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();

            std::cout << "Response:\n" << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace custom_headers_and_timeout
{
    void run()
    {
        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            std::ostringstream response;
            const std::list<std::string> headers = {
                "Accept: application/json",
                "User-Agent: curlpp-example/1.0"
            };

            request.setOpt(new curlpp::options::Url("https://httpbin.org/headers"));
            request.setOpt(new curlpp::options::HttpHeader(headers));
            request.setOpt(new curlpp::options::Timeout(10)); // 10 seconds
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();
            std::cout << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}


namespace follow_redirects
{
    void run()
    {
        try
        {
            std::ostringstream response;
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url("http://httpbin.org/redirect/2"));
            request.setOpt(new curlpp::options::FollowLocation(true));
            request.setOpt(new curlpp::options::Verbose(true));
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();
            std::cout << "Final Response:\n" << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace basic_authentication
{
    void run()
    {
        try
        {
            std::ostringstream response;

            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url("https://httpbin.org/basic-auth/user/passwd"));
            request.setOpt(new curlpp::options::UserPwd("user:passwd"));
            request.setOpt(new curlpp::options::WriteStream(&response));

            request.perform();
            std::cout << "Authenticated Response:\n" << response.str() << std::endl;
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace download_file_to_disk
{
    void run()
    {
        static constexpr std::string_view filePath {R"(/tmp/downloaded.txt)"};
        static constexpr std::string_view url {"https://httpbin.org/robots.txt"};
        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            std::ofstream file(filePath.data());
            request.setOpt(new curlpp::options::Url(url.data()));
            request.setOpt(new curlpp::options::WriteStream(&file));

            request.perform();
            file.close();
            std::println("Saved response to {}", filePath);
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace download_file_to_disk_progress_callback
{
    void run()
    {
        static constexpr std::string_view filePath {R"(/tmp/downloaded.txt)"};
        static constexpr std::string_view url {"https://httpbin.org/robots.txt"};

        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            std::ofstream file(filePath.data());
            request.setOpt(new curlpp::options::Url(url.data()));
            request.setOpt(new curlpp::options::WriteStream(&file));

            // Define a progress callback
            request.setOpt(new curlpp::options::ProgressFunction(
                [](const double total, const double curr, double uploadTotal, double uploadCurr) -> int {
                    if (total > 0) {
                        const double percent = (curr / total) * 100.0;
                        std::cerr << "\rDownloaded: " << curr << " (" << percent << "%)\n" <<  std::flush;
                    }
                    return 0; // return non-zero to abort
                }));

            request.setOpt(new curlpp::options::NoProgress(false)); // must be false to enable callback
            request.perform();

            file.close();
            std::println("Saved response to {}", filePath);
        }
        catch (curlpp::RuntimeError & e) {
            std::cerr << e.what() << std::endl;
        } catch (curlpp::LogicError & e){
            std::cerr << e.what() << std::endl;
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

void CurpCpp::TestAll()
{
    // simple_example();
    // verbose_callback::run();
    // get_info::run();
    // post_JSON_request::run();
    // multipart_upload::run();
    // custom_headers_and_timeout::run();
    // follow_redirects::run();
    // basic_authentication::run();
    // download_file_to_disk::run();
    download_file_to_disk_progress_callback::run();
}
