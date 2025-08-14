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
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <random>

#include "../Helpers/Integer.h"
#include "../Helpers/Helpers.h"
#include "PerfUtilities.h"

#include "UnorderedSet.h"

namespace
{
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

namespace UnorderedSet::HeterogeneousLookup
{
    using Helpers::Integer;

    struct Hash
    {
        using is_transparent = void;

        size_t operator()(int32_t value) const {
            return std::hash<int32_t>{}(value);
        }

        size_t operator()(const Integer& integer) const {
            return std::hash<int32_t>{}(integer.value);
        }
    };

    struct Comparator
    {
        using is_transparent = void;

        bool operator()(int64_t left, const Integer& right) const {
            return left == right.value;
        }

        bool operator()(const Integer& left, const Integer& right) const {
            return left.value == right.value;
        }
    };


    struct IntegerNoTransparentHash {
        std::size_t operator()(const Integer& s) const noexcept {
            return std::hash<int>{}(s.getValue());
        }
    };


    void Test_Bad()
    {
        std::unordered_set<Integer, IntegerNoTransparentHash> data;

        data.emplace(10);
        data.emplace(5);

        std::cout << " ---------------------- before find(5) -----------------------" <<  std::endl;
        decltype(data)::iterator  j = data.find(5);
        std::cout << " ---------------------- after -----------------------" <<  std::endl;

        std::cout << "Result: " << j->value << "\n";
    }

    void Test_OK()
    {
        std::unordered_set<Integer, Hash, Comparator> data;

        data.emplace(10);
        data.emplace(5);

        std::cout << " ---------------------- before find(5) -----------------------" <<  std::endl;
        decltype(data)::iterator j = data.find(5z);
        std::cout << " ---------------------- after -----------------------" <<  std::endl;

        std::cout << "Result: " << j->value << "\n";
    }
}


namespace UnorderedSet::HeterogeneousLookup_Hashing
{

    template <typename Hash>
    struct KeyHashPair
    {
        std::string_view key;
        std::size_t hash {};

        explicit KeyHashPair(std::string_view sv) :
                key { sv }, hash { Hash{}(key) } {
        }
    };

    struct Hash
    {
        using is_transparent = void;

        std::size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }

        std::size_t operator()(KeyHashPair<Hash> pair) const noexcept {
            return pair.hash;
        }
    };

    struct Comparator
    {
        using is_transparent = void;

        constexpr bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
            return lhs == rhs;
        }

        template <typename Hash>
        constexpr bool operator()(KeyHashPair<Hash> lhs, std::string_view rhs) const noexcept {
            return lhs.key == rhs;
        }
    };

    void Search()
    {
        using Set = std::unordered_set<std::string, Hash, Comparator>;

        Set set1 {"foo"};

        const std::string string {"foo"};
        const KeyHashPair<Set::hasher> pair {string};

        const bool exists = set1.contains(pair);
        std::cout << "Exist: " << std::boolalpha << exists << std::endl;
    }
}


namespace UnorderedSet::HeterogeneousLookup_Hashing_Integer
{
    using Helpers::Integer;


    template <typename Hash>
    struct KeyHashPair
    {
        Integer key;
        std::size_t hash {};

        explicit KeyHashPair(int k) :
                key { k }, hash { Hash{}(key) } {
        }
    };

    struct Hash
    {
        using is_transparent = void;

        std::size_t operator()(const Integer& v) const noexcept {
            return std::hash<int32_t>{}(v.value);
        }

        std::size_t operator()(const KeyHashPair<Hash>& pair) const noexcept {
            return pair.hash;
        }
    };

    struct Comparator
    {
        using is_transparent = void;

        constexpr bool operator()(const Integer& lhs, const Integer& rhs) const noexcept {
            return lhs == rhs;
        }

        template <typename Hash>
        constexpr bool operator()(const KeyHashPair<Hash>& lhs, const Integer& rhs) const noexcept {
            return lhs.key == rhs;
        }
    };

    void Search()
    {
        using Set = std::unordered_set<Integer, Hash, Comparator>;

        Set set1;
        set1.emplace(1);

        const KeyHashPair<Set::hasher> pair {1};

        std::cout << "=========================================================================\n";

        const bool exists = set1.contains(pair);
        std::cout << "Exist: " << std::boolalpha << exists << std::endl;

        std::cout << "=========================================================================\n";

    }
}


namespace UnorderedSet::MAP_vs_SET
{
	static std::random_device randomDevice{};
	static std::mt19937 generator(randomDevice());

	[[nodiscard]]
	int randomIntegerInRange(const int from = 0, const int until = 1000) {
		return std::uniform_int_distribution<int>{from, until}(generator);
	}

	[[nodiscard]]
	int getRandomUniqueInt(const int start = 0, const int end = 1000)
	{
		static std::unordered_set<int> uniqueInts;
		std::uniform_int_distribution intDistribution { std::uniform_int_distribution<>{ start, end } };

		while (true) {
			if (const int number = intDistribution(generator); uniqueInts.insert(number).second)
				return number;
		}
	}

	using Integer = Helpers::Wrapper<int, false>;

	template <typename Hash>
	struct KeyHashPair
	{
		Integer key;
		std::size_t hash {};

		explicit KeyHashPair(int k) :
				key { k }, hash { Hash{}(key) } {
		}
	};

	struct Hash
	{
		using is_transparent = void;

		std::size_t operator()(const Integer& v) const noexcept {
			return std::hash<int32_t>{}(v.value);
		}

		std::size_t operator()(const KeyHashPair<Hash>& pair) const noexcept {
			return pair.hash;
		}
	};

	struct Comparator
	{
		using is_transparent = void;

		constexpr bool operator()(const Integer& lhs, const Integer& rhs) const noexcept {
			return lhs == rhs;
		}

		template <typename Hash>
		constexpr bool operator()(const KeyHashPair<Hash>& lhs, const Integer& rhs) const noexcept {
			return lhs.key == rhs;
		}
	};

	__attribute__((optimize("O0")))
	void benchmark()
	{
		using Set = std::unordered_set<Integer, Hash, Comparator>;
		using Map = std::unordered_map<int, Integer>;

		constexpr int samplesCount { 10'000 }, iter { 100'000 };
		const std::vector<int> samples = []() {
			std::vector<int> tmp;
			tmp.reserve(samplesCount);
			for (int i = 0; i < samplesCount; ++i) {
				tmp.push_back(getRandomUniqueInt(1, 10 * samplesCount));
			}
			return tmp;
		}();

		Set set;
		Map map;
		for (int entry: samples) {
			set.emplace(entry);
			map.emplace(entry, entry);
		}

		{
			PerfUtilities::ScopedTimer timer { "SET" };
			for (int i = 0; i < iter; ++i) {
				for (int entry: samples) {
					const auto _ = set.contains(entry);
				}
			}
		}

		{
			PerfUtilities::ScopedTimer timer { "MAP" };
			for (int i = 0; i < iter; ++i) {
				for (int entry: samples) {
					const auto _ = map.contains(entry);
				}
			}
		}
	}

}

void UnorderedSet::TestAll()
{
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
    // PairsSet::InsertUnorderedPair_To_Set();


    // HeterogeneousLookup::Test_Bad();
    // HeterogeneousLookup::Test_OK();

    // HeterogeneousLookup_Hashing::Search();
    // HeterogeneousLookup_Hashing_Integer::Search();


	MAP_vs_SET::benchmark();
}