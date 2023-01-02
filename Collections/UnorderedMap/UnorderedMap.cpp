//============================================================================
// Name        : UnorderedMap_Testing.cpp
// Created on  : 15.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Unordered Map container testing
//============================================================================

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <string_view>
#include <cassert>
#include <variant>
#include <chrono>
#include <array>

#include "../Integer/Integer.h"
#include "UnorderedMap.h"

namespace UnorderedMap {

	// TODO: Make it work
	/*
	template <class T, typename KeyType, typename ValueType>
	//typename std::enable_if<std::is_base_of<std::map<KeyType, ValueType>, T>::value, T*>::type
	void print_map_s(const T& dictionary) {
		static_assert(std::is_base_of<std::map<KeyType, ValueType>, T>::value);
		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
	}
	*/

	template<typename Key, typename _Value>
	std::ostream& operator<<(std::ostream& stream, const std::unordered_map<Key, _Value>& map) {
		for (const auto& [k, v] : map)
			stream << "{" << k << ", " << v << "} ";
		return stream;;
	}

	template <typename T>
	void print_map(const T& dictionary) {
		for (auto& [key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
	}

	template <typename T>
	void print_map_info(const T& dictionary, std::string_view text = "") {
		std::cout << text << std::endl;
		print_map(dictionary);
	}

	struct IntegerHash {
		std::size_t operator()(const Integer& s) const noexcept {
			return std::hash<int>{}(s.getValue());
		}
	};

	std::array<std::string, 220> getCitiesArray() {
		std::array<std::string, 220> cities {"Aland Islands", "Albania", "Algeria", 
			"American Samoa", "Andorra", "Angola", "Anguilla", "Antarctica", "Antigua and Barbuda", 
			"Argentina", "Armenia", "Aruba", "Australia", "Austria", "Azerbaijan", "Bahrain", "Bahamas",
			"Bangladesh", "Barbados", "Belarus", "Belgium", "Belize", "Benin", "Bermuda", "Bhutan", "Bolivia",
			"Bonaire", "Bosnia and Herzegovina", "Botswana", "Bouvet Island", "Brazil", "British Indian Ocean Territory",
			"Brunei Darussalam", "Bulgaria", "Burkina Faso", "Burundi", "Cambodia", "Cameroon", "Canada", "Cape Verde",
			"Cayman Islands", "Central African Republic", "Chad", "Chile", "China", "Christmas Island",
            "Cocos (Keeling) Islands","Colombia", "Comoros", "Congo", "Congo, the Democratic Republic of the",
            "Cook Islands", "Costa Rica","Cte d'Ivoire",
			"Croatia", "Cuba", "Curaciao", "Cyprus", "Czech Republic", "Denmark", "Djibouti", "Dominica",
            "Dominican Republic", "Ecuador", "Egypt", "El Salvador", "Equatorial Guinea", "Eritrea", "Estonia", "Ethiopia",
            "Falkland Islands (Malvinas)","Faroe Islands", "Fiji", "Finland", "France", "French Guiana", "French Polynesia"
            "French Southern Territories", "Gabon","Gambia", "Georgia", "Germany", "Ghana", "Gibraltar",
            "Greece", "Greenland", "Grenada", "Guadeloupe", "Guam", "Guatemala",
			"Guernsey", "Guinea", "Guinea-Bissau", "Guyana", "Haiti", "Heard Island and McDonald Islands",
            "Holy See", "Honduras", "Hong Kong", "Hungary", "Iceland", "India", "Indonesia",
            "Iran", "Iraq", "Ireland", "Isle of Man", "Israel", "Italy", "Jamaica", "Japan",
			"Jersey", "Jordan", "Kazakhstan", "Kenya", "Kiribati", "Korea, Democratic People's Republic of",
            "Korea, Republic of", "Kuwait", "Kyrgyzstan", "Lao People's Democratic Republic",
            "Latvia", "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein", "Lithuania",
            "Luxembourg", "Macao", "Macedonia, the Former Yugoslav Republic of", "Madagascar", "Malawi", "Malaysia",
			"Maldives", "Mali", "Malta", "Marshall Islands", "Martinique", "Mauritania", "Mauritius", "Mayotte", "Mexico",
			"Micronesia, Federated States of", "Moldova, Republic of", "Monaco", "Mongolia", "Montenegro", "Montserrat",
			"Morocco", "Mozambique", "Myanmar", "Namibia", "Nauru", "Nepal", "Netherlands", "New Caledonia",
            "New Zealand", "Nicaragua", "Niger", "Nigeria", "Niue", "Norfolk Island", "Northern Mariana Islands",
            "Norway", "Oman", "Pakistan", "Palau", "Palestine, IState of", "Panama", "Papua New Guinea",
            "Paraguay", "Peru", "Philippines", "Pitcairn", "Poland", "Portugal", "Puerto Rico", "Qatar",
            "Reunion", "Romania", "Russian Federation", "Rwanda", "Saint Barthlemy",
            "Saint Helena, Ascension and Tristan da Cunha", "Saint Kitts and Nevis", "Saint Lucia", "Saint Martin (French part)",
            "Saint Pierre and Miquelon", "Saint Vincent and the Grenadines","Samoa", "San Marino", "Sao Tome and Principe",
            "Saudi Arabia", "Senegal", "Serbia", "Seychelles", "Sierra Leone", "Singapore", "Sint Maarten (Dutch part)",
            "Slovakia", "Slovenia", "Solomon Islands", "Somalia", "South Africa",
            "South Georgia and the South Sandwich Islands", "South Sudan", "Spain", "Sri Lanka", "Sudan",
            "Suriname", "Svalbard and Jan Mayen", "Swaziland", "Sweden", "Switzerland", "Syrian Arab Republic"
		};
		return cities;
	}
}

namespace UnorderedMap
{
	void Constructors_Tests() {
		constexpr auto merge =
                [](std::unordered_map<std::string, std::string> map1,std::unordered_map<std::string, std::string> map2) {
			std::unordered_map<std::string, std::string> temp(map1);
			temp.insert(map2.begin(), map2.end());
			return temp;
		};


		{
			std::cout << "Test #1:" << std::endl;
			std::unordered_map<std::string, std::string> dict;

			// Insert Few elements in map
			dict.insert({ "One",   "Value_1" });
			dict.insert({ "Two",   "Value_2" });
			dict.insert({ "Three", "Value_3" });
			dict.insert({ "Four",  "Value_4" });
			dict.insert({ "Five",  "Value_5" });

			print_map_info(dict, "Map contains: ");
		}

		{
			std::cout << "\n\nTest #2:" << std::endl;
			std::unordered_map<std::string, std::string> dict({
                { "One",  "Value_1" }, { "Two", "Value_2" }, { "Three",  "Value_3" }
            });
			print_map_info(dict, "Map contains: ");
		}

		{
			std::cout << "\n\nTest #3:" << std::endl;
			std::unordered_map<std::string, std::string> dict1({
                { "One",   "Value_1" }, { "Two",  "Value_2" },
                });
			std::unordered_map<std::string, std::string> dict2({ { "Three", "Value_3" }, { "Four", "Value_4" } });
			std::unordered_map<std::string, std::string> dict(merge(dict1, dict2));
			print_map_info(dict, "Map contains: ");
		}

		{
			std::cout << "\n\nTest #4:" << std::endl;
			std::unordered_map<std::string, std::string> dict1({ { "One",   "Value_1" }, { "Two",  "Value_2" }, });
			std::unordered_map<std::string, std::string> dict(dict1.begin(), dict1.end());
			print_map_info(dict, "Map contains: ");
		}
	}

	void Extract() {
		std::unordered_map<int, Integer> dict;

		dict.emplace(1, 1);
		dict.emplace(2, 2);
		dict.emplace(3, 3);
		dict.emplace(4, 4);
		dict.emplace(5, 5);

		print_map_info(dict, "Before: ");

		if (auto node = dict.extract(3); !node.empty()) {
			node.key() = 33;
			node.mapped().setValue(33333);

			auto [iter, b, c] = dict.insert(std::move(node));
			std::cout << "Inserted: {" << iter->first << " = " << iter->second << "}" << std::endl;
			std::cout << typeid(iter).name() << std::endl;
		}

		if (auto node = dict.extract(13); node.empty()) {
			std::cout << "Oppsss\n";
		}
		print_map_info(dict, "\nAfter: ");
	}


    void Extract_Test1() {
        std::unordered_map<int, Integer> dict;

        dict.emplace(1, 1);
        dict.emplace(2, 2);
        dict.emplace(3, 3);

        std::cout << dict << std::endl;

        auto entryOne = dict.extract(1);
        entryOne.key() = 33;
        entryOne.mapped().setValue(12345);

        dict.insert(std::move(entryOne));

        std::cout << dict << std::endl;
    }


	void Extract_Insert_ReturnType() {
		std::unordered_map<int, Integer> dict;

		dict.emplace(1, 1);
		dict.emplace(2, 2);

		auto node = dict.extract(2);
		node.key() = 22;
		node.mapped().setValue(2222);


		auto entry = dict.insert(std::move(node));
		std::cout << "inserted: " << std::boolalpha << entry.inserted << std::endl;
		// std::cout << "node: {" << entry.node.key() << " = " << entry.node.mapped() << "}" << std::endl;
		std::cout << "Iter: [" << entry.position->first << " = " << entry.position->second << "}" << std::endl;

	}

	void Reserve() {
		std::unordered_map<Integer, std::string, IntegerHash> dict;
		dict.reserve(10);

		std::cout << dict.size() << std::endl;
	}

    void Insert()
    {
        std::unordered_map<int, int> map;

        {
            const auto &[iter, ok] = map.insert({1, 1});
            std::cout << "ok = " << std::boolalpha << ok << " [" << iter->first << ", " << iter->second << "]\n";
            std::cout << map << std::endl;
        }

        {
            const auto &[iter, ok] = map.insert({1, 2});
            std::cout << "ok = " << std::boolalpha << ok << " [" << iter->first << ", " << iter->second << "]\n";
            std::cout << map << std::endl;
        }
    }

	void Emplace() {
		std::unordered_map<std::string, Integer> dict;

		dict.emplace("One", 1);
		dict.emplace("Two", 2);
		dict.emplace("Three", 3);
		dict.emplace("Five", 5);

		std::cout << "\nConstrucor for Integer(4) will be called:" << std::endl;
		dict.emplace("Four", 4);

		std::cout << "\nConstrucor for Integer(5) WILL called..... But " << std::endl;
		auto [iter, ok] = dict.emplace("Five", 5);
		if (false == ok) {
			std::cout << "Record already exists:" << iter->first << "  " << iter->second << std::endl;
		}

		std::cout << "\nConstrucor for Integer(5) WILL NOT !!!! be called with (try_emplace)..... Even if  " << std::endl;
		auto [iter1, ok1] = dict.try_emplace("Five", 5);
		if (false == ok1) {
			std::cout << "Record already exists:" << iter->first << "  " << iter->second << std::endl;
		}

		std::cout << std::endl;
	}

	struct Item {
		int x{ 0 };
		std::string val{ "None" };

		Item(int a, const std::string& s) : x{ a }, val{ s } {
			std::cout << "Item(int a, const std::string& s)\n";
		}

		Item(int a, std::string&& s) noexcept : x{ std::move(a) }, val{ std::move(s) } {
			std::cout << "Item(int a, std::string&& s) noexcept\n";
		}


		Item(const Item& item) : x{ item.x }, val{ std::move(item.val) } {
			std::cout << "Item(const std::string&& s)\n";
		}

		Item(Item&& item)  noexcept :
			x{ std::move(item.x) }, val{ std::move(item.val) } {
			std::cout << "Item(Item&& item)\n";
		}

		Item& operator=(const Item& item) {
			std::cout << "Item& operator=(const Item& item)" << std::endl;
			if (this == &item) {
				return *this;
			}
			x = item.x;
			val = item.val;
			return *this;
		}

		Item& operator=(Item&& item) noexcept {
			std::cout << "Item& operator=(Item&& item) noexcept" << std::endl;
			x = std::move(item.x);
			val = std::move(item.val);
			return *this;
		}
	};


	void Emplace_Existing() {
		std::unordered_map<int, Integer> map {
			{1, 1}
		};

        std::cout << "------------------------ emplace ----------------------------\n";
        {
            const auto &[iter, ok] = map.emplace(1, 222);
            std::cout << "ok = " << std::boolalpha << ok << " [" << iter->first << ", " << iter->second << "]\n";
            std::cout << map << std::endl;
        }

        std::cout << "------------------------ try_emplace ----------------------------\n";
        {
            const auto& [iter, ok] = map.try_emplace(1, 333);
            std::cout << "ok = " << std::boolalpha << ok << " [" << iter->first << ", " << iter->second << "]\n";
            std::cout << map << std::endl;
        }
        std::cout << "------------------------ ---- ----------------------------\n";
	}

	struct Resource {
		/** Resource data. **/
		std::string data{ "" };

		/** Resource mime content type: **/
		std::string content_type{ "" };

		Resource() = default;

		Resource(std::string data, std::string type) :
			data(std::move(data)), content_type(std::move(type)) {
		}
	};

	void Insert_vs_FindAndInsert() {
		{
			std::unordered_map<int, Integer> dict;
			for (auto val : { 1,2,2,3,3}) {
				if (auto i = dict.find(val); dict.end() == i) {
					dict.emplace(val, val);
				}
			}
		}

		std::cout << "---------------------------- Test2--------------------------------\n";

		{
			std::unordered_map<int, Integer> dict;
			for (auto val : { 1,2,2,3,3 }) {
				dict.emplace(val, val);
			}
		}

	}

	void Insert_CustomType() {
		std::unordered_map<std::string, Resource> dict;

		Resource res = { "1", "I" };
		auto& [a, b] = res;

		std::cout << res.data << "   " << res.content_type << std::endl;
		std::cout << a << "   " << b << std::endl;

		auto [iter, success] = dict.insert({ "One", res });
		std::cout << std::boolalpha << success << std::endl;
		std::cout << iter->second.data << "   " << iter->second.content_type << std::endl;

		// dict.emplace("Two", { "2", "II" });
		// dict.emplace("Three",{ "3", "III"});

		/*
		std::cout << "\nConstrucor for Integer(4) will be called:" << std::endl;
		dict.emplace("Four", 4);

		std::cout << "\nConstrucor for Integer(5) WILL called..... But " << std::endl;
		auto [iter, ok] = dict.emplace("Five", 5);
		if (false == ok) {
			std::cout << "Record already exists:" << iter->first << "  " << iter->second << std::endl;
		}
		*/

		std::cout << std::endl;
	}


	void Try_Emplace() {
		std::unordered_map<std::string, Integer> dict;

		dict.emplace("One", 1);
		dict.emplace("Two", 2);
		dict.emplace("Three", 3);
		dict.emplace("Five", 5);

		std::cout << "\nConstrucor for Integer(4) will be called:" << std::endl;
		dict.try_emplace("Four", 4);

		std::cout << "\nConstrucor for Integer(5) will NOT be called:" << std::endl;
		auto [iter, ok] = dict.try_emplace("Five", 5);
		if (false == ok) {
			std::cout << "Record already exists:" << iter->first << "  " << iter->second << std::endl;
		}

		std::cout << std::endl;
	}

    struct Data {
        int a {};
        int b {};

        Data() {
            std::cout << "Data::Data(" << a << "," << b  << ")" << std::endl;
        }

        Data(int i, int j = 0): a(i) {
            std::cout << "Data::Data(" << a << "," << b  << ")" << std::endl;
        }

        ~Data() {
            std::cout << "Data::~Data(" << a << "," << b  << ")" << std::endl;
        }
    };

    void Try_Emplace_CustomType() {
        std::unordered_map<std::string, Data> dict;

        dict.try_emplace("One", 1);
        std::cout << "---------------------------------------------------\n";

        dict.try_emplace("One", 2); // NOTE: Constructor WILL not be called
        std::cout << "---------------------------------------------------\n";


        for (const auto& [key, data]: dict)
            std::cout << key << ": Data(" << data.a << ", " << data.b << ")\n";
        std::cout << "---------------------------------------------------\n";
    }

	class Pool {
	private:
		std::unordered_map<int, std::shared_ptr<Integer>> storage;

	public:
		std::shared_ptr<Integer> getInteger(int value) {
			// BAD: Find & Emplace both search in list
			//return storage.try_emplace(value, std::make_shared<Integer>(value)).first->second;
			if (auto iter = storage.find(value); storage.end() != iter)
				return iter->second;
			return storage.emplace(value, std::make_shared<Integer>(value)).first->second;
		}
	};

	void Try_Emplace_Ptr() {
		Pool pool;

		std::shared_ptr<Integer> int1 = pool.getInteger(1);
		std::shared_ptr<Integer> int2 = pool.getInteger(1);
		std::shared_ptr<Integer> int3 = pool.getInteger(1);
	}


	void Contains() {
		const std::unordered_map<int, std::string> map{
			{1, "One"},
			{2, "Two"},
			{3, "Three"}
		};

		std::cout << "Map contains '2': " << std::boolalpha << map.contains(2) << std::endl;
	}

	void At() {
		const std::unordered_map<int, std::string> map{
				{1, "One"},
				{2, "Two"},
				{3, "Three"}
		};

		auto x = map.at(52);
		std::cout << x << std::endl;
	}

	void Count() {
		const std::unordered_map<int, std::string> map {
			{1, "One"},
			{2, "Two"},
			{3, "Three"}
		};

		std::cout << "Count of '2': " << map.count(2) << std::endl;
		for (const auto& [k, v] : map) {
			std::cout << k << " = " << v << std::endl;
		}
	}

	void Map_of_Variants() {
		using Key = std::variant<int, std::string>;
		using Value = std::string;

		std::unordered_map<Key, Value> kvs;

		kvs[Key{ 1 }] = "One";
		kvs[Key{ "1" }] = "One";

		std::cout << kvs[Key{ 1 }] << " " << kvs[Key{ "1" }] << "\n"; //One One

	}

	void Erase() {
		std::unordered_map<std::string, Integer> dict;
		dict.emplace("One", 1);
		dict.emplace("Two", 2);
		dict.emplace("Three", 3);


		auto size = dict.erase("Two");
		std::cout << size << " items deleted\n";

		for (const auto& [key, value] : dict) {
			std::cout << key << " = " << value.getValue() << std::endl;
		}
	}


	void Erase_Loop() {

		{
			std::unordered_map<int, int> dict{
				{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}
			};

			for (const auto& [key, value] : dict) {
				if (3 == key)
					dict.erase(key);
			}

			std::cout << "size = " << dict.size() << std::endl;
			std::for_each(dict.cbegin(), dict.cend(), [](const auto& e) {
                std::cout << e.first << " = " << e.second << std::endl;
            });
		}


		{
			std::unordered_map<int, int> dict{
				{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}
			};

			for (auto it = dict.cbegin(); dict.cend() != it;) {
				if (3 == it->first) {
					dict.erase(it++);
				}
				else ++it;

			}

			std::cout << "size = " << dict.size() << std::endl;
			std::for_each(dict.cbegin(), dict.cend(), [](const auto& e) {
                std::cout << e.first << " = " << e.second << std::endl;
            });
		}
	}

	#pragma optimize( "", off )
	void MapLookup_Performance_KeyType()
	{
		const auto cities = getCitiesArray();

		std::unordered_map<std::string, int> map_str;
		std::unordered_map<std::string_view, int> map_str_view;
		for (auto& s : cities) {
			map_str.emplace(s, 1);
			map_str_view.emplace(std::string_view{ s }, 1);
		}

		constexpr size_t TESTS_COUNT {100'000};
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				for (auto& s : cities) {
					map_str.find(s);
				}
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}

		{
			auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				for (auto& s : cities) {
					map_str_view.find(std::string_view{ s });
				}
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}
	}
	#pragma optimize( "", on )

    void Tests() {
        std::unordered_map<int, std::string> dict;
        dict.emplace(1, "One");
        dict.emplace(2, "Two");
        dict.emplace(3, "Three");
    }
}

namespace UnorderedMap::TransparentComparators {

    struct StringHasher {
        using is_transparent = void;

        size_t operator()(const char* str) {
            return std::hash<const char*>{}(str);
        }

        size_t operator()(std::string_view str) {
            return std::hash<std::string_view>{}(str);
        }

        size_t operator()(const std::string& str) {
            return std::hash<std::string>{}(str);
        }
    };


    void MapWith_String_Key() {
        std::unordered_map<std::string, int, StringHasher, std::equal_to<>> dict {};
    }

    // INFO: Back up for Quick C++ Benchmark
    //       'CharArrayTransparent' show best results
    void PerformanceTest() {
#if 0
        #include <unordered_map>
        #include <string>
        #include <string_view>

        struct stringish_hash {
            using is_transparent = void;

            size_t operator()(const char* str) const        { return std::hash<const char*>{}(str); }
            size_t operator()(std::string_view str) const   { return std::hash<std::string_view>{}(str); }
            size_t operator()(std::string const& str) const { return std::hash<std::string>{}(str); }
        };

        constexpr size_t N = 100000;

        void gen(auto& m) {
            std::generate_n(std::inserter(m, m.begin()), N, [i=1]() mutable {
                return std::pair<std::string, int>{std::to_string(i), i++}; });
        }

        static void CharArrayNoTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int> m;
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find("666"));
          }
        }
        BENCHMARK(CharArrayNoTransparent);

        static void StringViewNoTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int> m;
          std::string_view sv {"666"};
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find(std::string(sv)));
          }
        }
        BENCHMARK(StringViewNoTransparent);

        static void StringNoTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int> m;
          std::string str {"666"};
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find(str));
          }
        }
        BENCHMARK(StringNoTransparent);

        static void CharArrayTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int, stringish_hash, std::equal_to<>> m;
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find("666"));
          }
        }
        BENCHMARK(CharArrayTransparent);

        static void StringViewTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int, stringish_hash, std::equal_to<>> m;
          std::string_view sv {"666"};
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find(sv));
          }
        }
        BENCHMARK(StringViewTransparent);

        static void StringTransparent(benchmark::IState& state) {
          std::unordered_map<std::string, int, stringish_hash, std::equal_to<>> m;
          std::string str {"666"};
          gen(m);
          for (auto _ : state) {
            benchmark::DoNotOptimize(m.find(str));
          }
        }
        BENCHMARK(StringTransparent);

#endif
    }
}


namespace UnorderedMap::InvalidateIterators
{

    struct Entry {
        std::map<int, std::string> dict {};
    };

    void CheckValueIterators_MAP()
    {
        const std::string testBookName { "Book_3" };
        std::unordered_map<std::string, Entry> book;

        book.try_emplace("Book_1");
        book.try_emplace("Book_2");

        auto [bookIter, ok] = book.try_emplace(testBookName );

        {
            bookIter->second.dict.emplace(1, "One");
            bookIter->second.dict.emplace(2, "Two");
            bookIter->second.dict.emplace(3, "Three");
            bookIter->second.dict.emplace(4, "Four");
            bookIter->second.dict.emplace(5, "Five");
        }

        auto iter = bookIter->second.dict.find(3);

        for (int i = 4;  i < 1000; ++i)
            book.try_emplace("Book_" + std::to_string(i));



        if (bookIter == book.end()) {
            std::cout << "END" << std::endl;
        }
        else {
            std::cout << bookIter->second.dict.size() << std::endl;
            std::cout << iter->second << std::endl;
        }

        if (bookIter == book.find(testBookName)) {
            std::cout << "Iter match for " << testBookName << std::endl;
        }
    }
}

void UnorderedMap::TEST_ALL()
{
	// Constructors_Tests();
	// Reserve();

	// Insert();
	// Emplace();
	// Emplace_Existing();

	// Contains();

	// At();

	// Count();

	// Insert_vs_FindAndInsert();
	// Insert_CustomType();



	// Erase();
	// Erase_Loop();

	// Try_Emplace();
    Try_Emplace_CustomType();
	// Try_Emplace_Ptr();

	// Extract();
    // Extract_Test1();
	// Extract_Insert_ReturnType();

	// Delete_InLoop();

	// Map_of_Variants();

	// MapLookup_Performance_KeyType();


    // TransparentComparators::MapWith_String_Key();
    // TransparentComparators::PerformanceTest();

    // InvalidateIterators::CheckValueIterators_MAP();


    // Tests();
}