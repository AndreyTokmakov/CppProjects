//============================================================================
// Name        : ThreadHelperUtilities.h
// Created on  : 22.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Thread helper utilities src
//============================================================================

#ifndef THREAD_HELPER_UTILITIES_INCLUDE_GUARD_H_
#define THREAD_HELPER_UTILITIES_INCLUDE_GUARD_H_

#include <iostream>
#include <thread>

namespace ThreadHelperUtilities {

	class ThreadInfo {
	protected:
		std::thread::id this_thread_id;

	protected:
		const std::string getCurrentTime() const;

	public:
		ThreadInfo();
		friend std::ostream& operator<< (std::ostream& stream, const ThreadInfo& threadInfo);
	};
};

#endif /* THREAD_HELPER_UTILITIES_INCLUDE_GUARD_H_ */