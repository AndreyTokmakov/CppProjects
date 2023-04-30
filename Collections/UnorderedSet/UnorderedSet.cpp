//============================================================================
// Name        : Unordered_Set_Testing.cpp
// Created on  : 15.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Unordered Set container testing
//============================================================================

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <string>
#include <string_view>
#include <cassert>
#include <vector>

#include "../Integer/Integer.h"
#include "UnorderedSet.h"

namespace UnorderedSet {
	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_set = [](const auto& set) {
		std::for_each(set.begin(), set.end(), printer_coma);
	};

	auto print_set_ex = [](const auto& set,
		std::string_view text,
		std::string_view postfix = "\n") {
		std::cout << text;
		print_set(set);
		std::cout << postfix << std::endl;
	};

	struct IntegerHash {
		std::size_t operator()(const Integer& s) const noexcept {
			return std::hash<int>{}(s.getValue());
		}
	};

	struct IntegerBadHash {
		std::size_t operator()([[maybe_unused]] const Integer& s) const noexcept {
			return std::hash<int>{}(12345);
		}
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::unordered_set<T>& set) {
		for (const T& e : set)
			stream << e << ' ';
		return stream;
	}

	template<typename Hash>
	std::ostream& operator<<(std::ostream& stream, const std::unordered_set<Integer, Hash>& set) {
		for (const Integer& e : set)
			stream << e.getValue() << ' ';
		return stream;
	}
}

namespace UnorderedSet {

	void Constructors() { 
		{
			std::unordered_set<int> ints_set{ 1,2,3,4,5,6,7,8,9 };
			print_set_ex(ints_set, "Set contains: ");
		}
		{
			std::vector<int> v = {1,2,3,4,5,6};
			std::unordered_set<int> ints_set(v.begin(), v.end());
		}
	}

	void Insert() {
		std::unordered_set<Integer, IntegerHash> numbers;
		int i = 5;
		while (i--) {
			numbers.insert(i);
		}
		std::cout << "Set contains: " << std::endl;
		std::cout << numbers << std::endl;
	}



	void Emplace() {
		std::unordered_set<Integer, IntegerHash> numbers;
		int i = 5;
		while (i--) 
			numbers.emplace(i);
		
		std::cout << "Set contains: " << std::endl;
		std::cout << numbers << std::endl;
	}


	void Emplace_Return() {
		std::unordered_set<Integer, IntegerHash> numbers;
		

		numbers.emplace(1);
		numbers.emplace(2);
		auto [iter, ok] = numbers.emplace(3);


		std::cout << "\n" << iter->getValue()<< std::endl;
		std::cout << std::boolalpha << ok << std::endl;

		std::cout << "\nSet contains: " << std::endl;
		std::cout << numbers << std::endl;
	}

	void Emplace_Hint() {
		std::unordered_set<Integer, IntegerHash> numbers;
		int i = 5;
		while (i--)
			numbers.emplace_hint(numbers.end(), i);
		
		std::cout << "Set contains: " << std::endl;
		std::cout << numbers << std::endl;
	}




	void Erase() {
		std::unordered_set<Integer, IntegerHash> numbers;
		for (int i : {1,2,3,4,5,6,8,9,0}) 
			numbers.emplace(i);
		
		print_set_ex(numbers, "Before: ");

		for (int i : {1, 2, 3, 4}) {
			size_t count = numbers.erase(i);
			std::cout << "Numbers deleted = " << count << std::endl;
			print_set_ex(numbers, "After: ");
		}
	}

	void Erase_Iter() {
		std::unordered_set<Integer, IntegerHash> numbers;
		for (int i : {1, 2, 3})
			numbers.emplace(i);

		print_set_ex(numbers, "Before: ");

		numbers.erase(numbers.begin());
		print_set_ex(numbers, "After: ");
	}

	void Erase_IterRange() {
		std::unordered_set<Integer, IntegerHash> numbers;
		for (int i : {1,2,3,4,5,6,7,8,9})
			numbers.emplace(i);

		print_set_ex(numbers, "Before: ");

		numbers.erase(numbers.find(3), numbers.find(7));

		print_set_ex(numbers, "After: ");
	}

	void Extract() {
		std::unordered_set<Integer, IntegerHash> src, dst;
		for (int i : {1, 3, 5})
			src.emplace(i);

		std::cout << "tests: " << src << "\ndst: " << dst << std::endl;

		for (int i : {2,4,6}) {
			auto entry = src.extract(i);
			//dst.insert(std::move(entry));
		}

		std::cout << "tests: " << src << "\ndst: " << dst << std::endl;
	}

	void Find() {
		std::unordered_set<Integer, IntegerHash> numbers;
		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			numbers.emplace(i);
		
		std::cout << std::endl;

		auto result = numbers.find(4);
		std::cout << *result << std::endl;

		std::cout << std::endl;
	}

	void Count() {
		std::unordered_set<Integer, IntegerHash> numbers;
		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			numbers.emplace(i);
			
		size_t count = numbers.count(3);
		std::cout << "Count elements with key = 3 is " << count << std::endl;
	}

	void Count1() {
		std::unordered_set<Integer, IntegerBadHash> numbers;
		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			numbers.emplace(i);

		size_t count = numbers.count(3);
		std::cout << "Count elements with key = 3 is " << count << std::endl;
	}

	void Swap() {
		std::unordered_set<Integer, IntegerHash> nums1, nums2;
		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
			nums1.emplace(i);
			nums2.emplace(i * 2) ;
		}

		print_set_ex(nums1, "nums1: ");
		print_set_ex(nums2, "nums2: ");

		nums1.swap(nums2);
		print_set_ex(nums1, "nums1: ");
		print_set_ex(nums2, "nums2: ");
	}

	void Hash_Function() {
		std::unordered_set<Integer, IntegerHash> nums1;
		for (int i : {1, 2, 3})
			nums1.emplace(i);

		const auto func = nums1.hash_function();

		std::cout << "hash_function(" <<123 << ") = " << func(123) << std::endl;
	}

	void Delete_InLoop() {
		std::unordered_set<int> set{1,2,3,4,5,6,7,8,9};

		std::cout << set << std::endl;

		for (auto i = set.begin(); i != set.end();) {
			if (*i == 7) {
				set.erase(i++);
			}
			else {
				++i;
			}
		}

		std::cout << set << std::endl;
	}
}

namespace UnorderedSet::PairsSet
{
    template<typename T>
    struct PairHash {
        std::size_t operator()(const std::pair<T,T>& pair) const noexcept {
            std::size_t h1 = std::hash<T>{}(pair.first);
            std::size_t h2 = std::hash<T>{}(pair.second);
            return h1 ^ (h2 << 1); // or use boost::hash_combine
        }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::pair<T, T>& pair) {
        stream << "(" << pair.first << ", " << pair.second << ")";
        return stream;
    }

    //---------------------------------------------------------------------------------


    template<typename T>
    struct UnorderedPair: std::pair<T, T> {
        UnorderedPair(T a, T b): std::pair<T, T> {a, b} {
            ///
        }

        template<typename _T>
        friend std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair);

        template<typename _T>
        friend bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2);

        /*
        template<typename _T>
        friend bool operator<(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2);
        */
    };

    template<typename T>
    struct PairHashUnordered {
        std::size_t operator()(const UnorderedPair<T>& pair) const noexcept {
            std::size_t h1 = std::hash<T>{}(std::min(pair.first, pair.second));
            std::size_t h2 = std::hash<T>{}(std::max(pair.first, pair.second));
            return h1 ^ (h2 << 1);
        }
    };

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair) {
        stream << "(" << pair.first << ", " << pair.second << ")";
        return stream;
    }

    /*
    template<typename _T>
    bool operator<(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2) {
        return (p1.first + p1.second) < (p2.first + p2.second);
    }
    */

    template<typename _T>
    bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2) {
        return (p1.first == p2.first && p1.second == p2.second) ||
               (p1.second == p2.first && p1.first == p2.second);
    }

    void InsertPair_To_Set()
    {
        using T = int;
        using Type = std::pair<T, T>;

        std::unordered_set<Type, PairHash<T>> pairsSet;
        pairsSet.emplace(1,1);
        pairsSet.emplace(2,1);
        pairsSet.emplace(1,2);

        for (const auto& P: pairsSet)
            std::cout << P << std::endl;
    }

    void InsertUnorderedPair_To_Set()
    {

        std::unordered_set<UnorderedPair<int>, PairHashUnordered<int>> pairsSet;
        auto [iter, ok ] = pairsSet.emplace(2,1);
        auto [iter1, ok1 ] = pairsSet.emplace(1,2);

        std::cout << std::boolalpha << ok << std::endl;
        std::cout << std::boolalpha << ok1 << std::endl;

        for (const auto& P: pairsSet)
            std::cout << P << std::endl;
    }
}

void UnorderedSet::TEST_ALL() {
	// Constructors();

	// Insert();

	// Find();
	// Count();
	// Count1();

	// Emplace();
	// Emplace_Return();
	// Emplace_Hint();

	// Erase();
	// Erase_Iter();
	// Erase_IterRange();

	// Extract();
	// Swap();

	// Hash_Function();

	// Delete_InLoop();


    // PairsSet::InsertPair_To_Set();
    PairsSet::InsertUnorderedPair_To_Set();
}