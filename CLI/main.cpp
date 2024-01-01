/**============================================================================
Name        : CLI.cpp
Created on  : 15.09.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Experimentak CLI
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <functional>

namespace
{
    using namespace std::string_view_literals;

    struct CLI
    {
        explicit CLI(std::basic_ostream<char> &out = std::cout) : outStream{out} {
        }

        enum class Status {
            Ok,
            Continue, /**< Continue processing */
            Stop      /**< Stop processing */
        };

        void start() {
            Status status = Status::Ok;
            std::string input;
            while (Status::Stop != status) {
                outStream << "> ";
                std::getline(std::cin, input);
                status = processCommand(input);
            }
        }

    private: /** Handlers **/

        [[nodiscard]]
        Status info(std::string_view name)
        {
            outStream << "info" << std::endl;

            return Status::Ok;
        }

        [[nodiscard]]
        Status exit(std::string_view name)
        {
            outStream << name << std::endl;

            return Status::Stop;
        }

    private:

        using CmdHandlerType = CLI;
        using methodPtr_t = Status (CmdHandlerType::*)(std::string_view params);

        std::basic_ostream<char>& outStream;


        template<typename... Args>
        auto calFunction(methodPtr_t func, Args&&... params) -> decltype(auto)
        {
            return std::invoke(func , this, std::forward<Args>(params)...);
        }

        [[nodiscard]]
        std::pair<std::string_view, std::string_view>
        extractCommand(std::string_view input) const noexcept
        {
            const size_t spaceIdx = input.find(' ');
            if (std::string_view::npos == spaceIdx)
                return std::make_pair(input, "");

            return std::make_pair(input.substr(0, spaceIdx),
                                  input.substr(spaceIdx + 1, input.size() - spaceIdx - 1));
        }

        [[nodiscard]]
        Status processCommand(std::string_view userInput)
        {
            const auto& [cmd, params] = extractCommand(userInput);
            if (const auto funcIter = funcMapping.find(cmd); funcMapping.end() != funcIter) {
                return calFunction(funcIter->second, params);
            }

            outStream << "Invalid command '" << cmd << "'\n";
            return Status::Continue;
        }

        static inline const std::unordered_map<std::string_view, methodPtr_t> funcMapping
        {
            {"info"sv, &CmdHandlerType::info},
            {"q"sv,    &CmdHandlerType::exit},
            {"quit"sv, &CmdHandlerType::exit},
            {"exit"sv, &CmdHandlerType::exit},
        };
    };
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // CLI cli {};
    // cli.start();



    return EXIT_SUCCESS;
}
