/**============================================================================
Name        : Simple_Coroutine_Waitable.cpp
Created on  : 28.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple Coroutine Waitable
============================================================================**/

#include "Coroutines.h"
#include "Utilities.h"

#include <coroutine>
#include <print>
#include <chrono>
#include <thread>
#include <fstream>

namespace {
    using Utilities::getCurrentTime;
}

namespace
{
    struct FileReader
    {
        struct promise_type
        {
            FileReader get_return_object()
            {
                std::println("[{}] get_return_object", getCurrentTime());
                return FileReader{this};
            }

            std::suspend_never initial_suspend() {
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                return {};
            }

            void return_void() {
            }

            void unhandled_exception() {
                std::terminate();
            }
        };

        promise_type* promise;


        FileReader(promise_type* p) : promise(p) {
            std::println("[{}] FileReader()", getCurrentTime());
        }

        ~FileReader() {
            std::println("[{}] ~FileReader()", getCurrentTime());
        }

        struct Awaiter
        {
            std::ifstream file;
            std::string line;

            bool await_ready() {
                return false;
            }

            void await_suspend(std::coroutine_handle<> h)
            {
                while (std::getline(file, line)) {
                    std::println("[{}] Processing {}", getCurrentTime(), line);
                    h.resume();
                }
                std::println("[{}] await_suspend done", getCurrentTime());
            }

            void await_resume() {
            }
        };

        Awaiter readAsync(const std::string& filename)
        {
            Awaiter awaiter;
            awaiter.file.open(filename);
            return awaiter;
        }
    };

    FileReader processFiles()
    {
        co_await "12345";

        //co_await FileReader{}.readAsync(R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/Coroutines/data/file1.txt)");
        //co_await FileReader{}.readAsync(R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/Coroutines/data/file2.txt)");
    }
}


void Coroutines::UseCases_FileReader_Coroutine::TestAll()
{
    processFiles();
}
