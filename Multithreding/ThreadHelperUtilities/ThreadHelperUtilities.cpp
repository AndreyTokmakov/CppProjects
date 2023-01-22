//============================================================================
// Name        : ThreadHelperUtilities.cpp
// Created on  : 22.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Thread helper utilities src
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <thread>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include "ThreadHelperUtilities.h"

namespace ThreadHelperUtilities {

    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::mutex print_mutex;

    std::string ThreadInfo::getCurrentTime() const noexcept {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        const auto nowMs = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch()) % 1000000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%a %b %d %Y %T")
           << '.' << std::setfill('0') << std::setw(6) << nowMs.count();
        return ss.str();
    }

    ThreadInfo::ThreadInfo() {
        this_thread_id = std::this_thread::get_id();
    }

    std::ostream& operator<< (std::ostream& stream, const ThreadInfo& threadInfo) {
        std::lock_guard<std::mutex> lock(print_mutex);
        stream << "[" << threadInfo.getCurrentTime() << "] Thread [";
        if (mainThreadId == threadInfo.this_thread_id) {
            stream << std::setiosflags(std::ios::left) << std::setw(9) << "Main";
        }
        else {
            stream << "Id: " << std::setiosflags(std::ios::left) << std::setw(5) << threadInfo.this_thread_id;
        }
        stream << "] ";
        stream.flush();;
        return stream;
    }
}