/**============================================================================
Name        : Performance.cpp
Created on  : 01.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Performance
============================================================================**/

#include "Performance.h"

#include <iostream>
#include <array>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>


namespace Utils
{
    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start {
                std::chrono::high_resolution_clock::now()
        };

        explicit ScopedTimer(std::string_view info) :
            benchmarkName {info} {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };
}


namespace Performance::Stack_Vector
{
    const size_t N = 1000;

    constexpr int op_push = 0;
    constexpr int op_pop = 0;
    constexpr size_t COUNT = 10'000'000;

    std::array<std::pair<int, int>, N> ops = []() {
        std::array<std::pair<int, int>, N> ops{};
        int pushed = 0;
        for (size_t i = 0; i != N; ++i) {
            int op = rand() % 2;
            if (op == op_pop && pushed > 0) {
                ops[i] = {op_pop, -1};
            } else { // (op == op_push)
                ops[i] = {op_push, rand()};
                pushed += 1;
            }
        }
        return ops;
    }();

    static void DefaultStack()
    {
        Utils::ScopedTimer timer {"DefaultStack"};
        for (size_t i = 0; i < COUNT; ++i) {
            std::stack<int> st;
            std::for_each(begin(ops), end(ops), [&st](const auto& op) {
                if (op.first == op_push) {
                    st.push(op.second);
                } else {
                    st.top(); st.pop();
                }
            });
        }
    }

    static void VectorStack()
    {
        Utils::ScopedTimer timer {"VectorStack"};
        for (size_t i = 0; i < COUNT; ++i) {
            std::stack<int, std::vector<int>> st;
            std::for_each(begin(ops), end(ops), [&st](const auto& op) {
                if (op.first == op_push) {
                    st.push(op.second);
                } else {
                    st.top(); st.pop();
                }
            });
        }
    }
};

void Performance::TestAll()
{
    Stack_Vector::DefaultStack();
    Stack_Vector::VectorStack();
};