//============================================================================
// Name        : PolymorphicMemoryResources.h
// Created on  : 12.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Polymorphic Memory Resources testsd 
//============================================================================

#include "PolymorphicMemoryResources.h"


#include "../Integer/Integer.h"
#include "../Memory/Memory.h"
#include <memory>
#include <new>
#include <iostream>
#include <string>
#include <memory_resource>
#include <cstdlib> // for std::byte

#include <vector>
#include <array>
#include <map>

namespace PolymorphicMemoryResources::AllocatingMemory {

	class Object {
	public:
		int value;

	public:
		Object(int v = 0) : value(v) {
			std::cout << "Object::Object(" << value << ") constructed at [" << this << "]" << std::endl;
		}
		~Object() {
			std::cout << "~Object::Object(" << value << ")" << std::endl;
		}

		friend std::ostream& operator<<(const std::ostream& stream, const Object& obj);
	};

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.value;
		return stream;
	}

	//---------------------------------------------------------------------------------//

	void NotAllocatingMemoryContainers() {

		// allocate some memory on the stack:
		std::array<std::byte, 200000> buffer;

		// and use it as initial memory pool for a vector:
		std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };
		std::pmr::vector<std::string> coll{ &pool };
		for (int i = 0; i < 1000; ++i) {
			coll.emplace_back("just a non-SSO string");
		}

		/*
		for (const std::string& str : coll) {
			std::cout << str << std::endl;
		}
		*/

		std::cout << "Done." << std::endl;
	}

	void NotAllocatingMemoryAtAll() {

		// allocate some memory on the stack:
		std::array<std::byte, 200000> buffer;

		// and use it as initial memory pool for a vector:
		std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };
		std::pmr::vector<std::pmr::string> coll{ &pool };
		for (int i = 0; i < 1000; ++i) {
			coll.emplace_back("just a non-SSO string");
		}

		for (const std::pmr::string& str : coll) {
			std::cout << str << std::endl;
		}

		std::cout << "Done." << std::endl;
	}

	void ReusingMemoryPools() {
		// allocate some memory on the stack:
		std::array<std::byte, 200000> buf;

		for (int num : {1000, 2000, 500, 2000, 3000, 50000, 1000}) {
			std::cout << "-- check with " << num << " elements:\n";
			std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size() };
			std::pmr::vector<std::pmr::string> coll{ &pool };
			for (int i = 0; i < num; ++i) {
				coll.emplace_back("just a non-SSO string");
			}
		}
	}
};

namespace PolymorphicMemoryResources::StandardMemoryResources {

	void NewDeleteResource() {
		std::pmr::string s1{ "my string", std::pmr::new_delete_resource() };
		std::cout << s1 << std::endl;

		std::pmr::synchronized_pool_resource pool1;
		std::pmr::string s2 { "String Pool1", &pool1 };
		std::cout << s2 << std::endl;

		std::pmr::monotonic_buffer_resource pool2;
		std::pmr::string s3 {"String Pool2", &pool2 };
		std::cout << s3 << std::endl;
	}

	void NewDeleteResource_Move() {
		std::string str {"my string with some value"};
		// copies
		std::pmr::string ps{ std::move(str), std::pmr::new_delete_resource() }; 
	}

	void DefaultMemoryResource() {
		static std::pmr::synchronized_pool_resource memPool;

		// set myPool as new default memory resource:
		std::pmr::memory_resource* old = std::pmr::set_default_resource(&memPool);

		// restore old default memory resource as default:
		std::pmr::set_default_resource(old);
	}
}


namespace PolymorphicMemoryResources::SynchronizedPool_PerformanceTests {

	void No_PMR_Allocation()
	{
		std::map<long, std::string> coll;
		for (int i = 0; i < 10; ++i) {
			std::string s { "Customer" + std::to_string(i) };
			coll.emplace(i, s);
		}

		// print element distances:
		for (const auto& elem : coll) {
			static long long lastVal = 0;
			long long val = reinterpret_cast<long long>(&elem);
			std::cout << "diff: " << (val - lastVal) << std::endl;
			lastVal = val;
		}
	}

	void With_PMR_Allocation()
	{
		std::pmr::synchronized_pool_resource pool;
		std::pmr::map<long, std::pmr::string> coll{ &pool };
		for (int i = 0; i < 10; ++i) {
			std::string s{ "Customer" + std::to_string(i) };
			coll.emplace(i, s);
		}
		// print element distances:
		for (const auto& elem : coll) {
			static long long lastVal = 0;
			long long val = reinterpret_cast<long long>(&elem);
			std::cout << "diff: " << (val - lastVal) << std::endl;
			lastVal = val;
		}
	}

	void HeapFragmentationTest() {
		No_PMR_Allocation();
		With_PMR_Allocation();
	}
}

void PolymorphicMemoryResources::TestAll() {
	
	// AllocatingMemory::NotAllocatingMemoryContainers();
	// AllocatingMemory::NotAllocatingMemoryAtAll();
	// AllocatingMemory::ReusingMemoryPools();

	// StandardMemoryResources::NewDeleteResource();
	// StandardMemoryResources::NewDeleteResource_Move();
	// StandardMemoryResources::DefaultMemoryResource();

	SynchronizedPool_PerformanceTests::HeapFragmentationTest();
};