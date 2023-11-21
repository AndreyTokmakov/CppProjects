#include "Map.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <cassert>
#include <ranges>

#include "../Integer/Integer.h"

namespace Map {

	using String = std::string;
	using CString = const String&;

	template <class T, typename KeyType, typename ValueType>
	//typename std::enable_if<std::is_base_of<std::map<KeyType, ValueType>, T>::value, T*>::type
	void print_map_s(const T& dictionary) {
		static_assert(std::is_base_of<std::map<KeyType, ValueType>, T>::value);
		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
	}

	template<typename K, typename V>
	std::ostream& operator<<(std::ostream& stream, const std::map<K,V>& map) {
		for (const auto& [k, v]: map)
			stream << k << " = " << v << std::endl;
		return stream;
	}


	/*
	template <class T,
			  typename KeyType,
			  typename ValueType>
	void print_map_ss(const typename std::enable_if<std::is_base_of<std::map<KeyType, ValueType>, T>::value>::type& dictionary) {
		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
	}
	*/
	template <typename T>
	void print_map(const T& dictionary) {
		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
	}

	template <typename T>
	void print_map_info(const T& dictionary, std::string_view text) {
		std::cout << text << std::endl;
		print_map(dictionary);
	}

    void emplace_duplicate_entry()
    {
        std::map<std::string, std::string> dict {
            {"One", "I"}, {"Two", "II"}, {"Three", "III"},
        };

        auto [iter, ok] = dict.emplace("Four", "IV");
        std::cout << "Result: [{" << iter->first << ", " << iter->second << "}, " << std::boolalpha << ok << "]\n\n";

        std::cout << dict << std::endl;

        std::tie(iter, ok) = dict.emplace("Four", "4");
        std::cout << "Result: [{" << iter->first << ", " << iter->second << "}, " << std::boolalpha << ok << "]\n\n";

        std::cout << dict << std::endl;
    }

    void try_emplace_duplicate_entry()
    {
        std::map<std::string, std::string> dict {
                {"One", "I"}, {"Two", "II"}, {"Three", "III"},
        };

        auto [iter, ok] = dict.try_emplace("Four", "IV");
        std::cout << "Result: [{" << iter->first << ", " << iter->second << "}, " << std::boolalpha << ok << "]\n\n";

        std::cout << dict << std::endl;

        std::tie(iter, ok) = dict.try_emplace("Four", "4");
        std::cout << "Result: [{" << iter->first << ", " << iter->second << "}, " << std::boolalpha << ok << "]\n\n";

        std::cout << dict << std::endl;
    }


    void emplace_test()
	{
		std::map<std::string, std::string> m;

		// uses pair's move constructor
		m.emplace(std::make_pair(std::string("a"), std::string("a")));

		// uses pair's converting move constructor
		m.emplace(std::make_pair("b", "abcd"));

		// uses pair's template constructor
		m.emplace("d", "ddd");

		// uses pair's piecewise constructor
		m.emplace(std::piecewise_construct,
			std::forward_as_tuple("c"),
			std::forward_as_tuple(10, 'c'));

		// for (const auto &p : m) 
		// 	std::cout << p.first << " => " << p.second << '\n';

		for (auto &[key, value] : m)
			std::cout << key << " => " << value << std::endl;
	}

    void emplace_test1() {
        std::map<int, std::pair<double, double>> metrics {};
        metrics.emplace(1, std::make_pair(1.0f, 1.0f));
    }

	void emplace_return() {
		std::map<std::string, std::string> dict;

		dict.emplace("key1", "value1");
		dict.emplace("key2", "value2");
		auto [iter, ok] = dict.emplace("key3", "value3");

		for (const auto& entry : dict)
			std::cout << entry.first << " = " << entry.second << std::endl;

        iter->second.append("_11").append("_22");

		std::cout << std::endl;
		for (const auto& entry : dict)
			std::cout << entry.first << " = " << entry.second << std::endl;
	}

	void emplace_vs_insert()
	{
		std::map<std::string, Integer> dictionary;

		std::cout << "Test using 'emplace()'. No copy construcors shall be called.\n" << std::endl;

		dictionary.emplace("Key1", 10);
		dictionary.emplace("Key2", 20);
		dictionary.emplace("Key3", 30);

		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
		dictionary.clear();

		std::cout << "\n\nTest using '()'. Copy construcors SHALL be called. (TEST1)\n" << std::endl;

		dictionary.insert(std::make_pair<String, Integer>("Key1", Integer(10)));
		dictionary.insert(std::make_pair<String, Integer>("Key2", Integer(20)));
		dictionary.insert(std::make_pair<String, Integer>("Key3", Integer(30)));

		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
		dictionary.clear();

		std::cout << "\n------------------------TEST 3 -----------------------------\n" << std::endl;

		dictionary.insert(std::pair("Key1", 10));
		dictionary.insert(std::pair("Key2", 20));
		dictionary.insert(std::pair("Key3", 30));

		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
		dictionary.clear();
	}

	void try_emplace_vs_emplace() {
		std::map<std::string, Integer> dictionary;
		std::cout << "************************ emplace() test ***********************\n" << std::endl;

		dictionary.emplace("Key1", 10);
		dictionary.emplace("Key2", 20);
		dictionary.emplace("Key3", 30);
		std::cout << ">>> Using 'emplace' to add an element Integer(40) with existing key 'Key3' <<<" << std::endl;
		dictionary.emplace("Key3", 40);

		print_map_info(dictionary, ">>> Collection contains:\n");
		std::cout << "\n\n" << std::endl;
		dictionary.clear();

		std::cout << "\n************************ try_emplace() test ***********************\n" << std::endl;

		dictionary.emplace("Key1", 10);
		dictionary.emplace("Key2", 20);
		dictionary.emplace("Key3", 30);
		std::cout << ">>> Using 'try_emplace' to add an element Integer(40) with existing key 'Key3' <<<" << std::endl;
		dictionary.try_emplace("Key3", 40);

		print_map_info(dictionary, ">>> Collection contains:\n");
		std::cout << "\n\n" << std::endl;
	}


	void emplace_test_2_ReferenceWrapper()
	{
		std::cout << "Test using 'emplace()'. No copy construcors shall be called.\n" << std::endl;
		std::map<std::string, Integer> dictionary;

		dictionary.emplace("Key1", 10);
		dictionary.emplace("Key2", 20);
		dictionary.emplace("Key3", 30);

		for (auto &[key, value] : dictionary)
			std::cout << key << "  ==  " << value << std::endl;
		dictionary.clear();


		std::cout << "\n\nTest using 'std::reference_wrapper<Integer>'. No copy construcors shall be called.\n" << std::endl;
		std::map<std::string, std::reference_wrapper<Integer>> dictionary_refs;
		//std::map<std::string, Integer> dictionary_refs;

		Integer int1(10);
		Integer int2(20);
		Integer int3(30);

		dictionary_refs.insert_or_assign("Key1", int1);
		dictionary_refs.insert_or_assign("Key2", int2);
		dictionary_refs.insert_or_assign("Key3", int3);

		for (auto &[key, value] : dictionary_refs)
			std::cout << key << "  ==  " << value << std::endl;

		int2.setValue(22);
		std::cout << "\nPrint map again:" << std::endl;
		for (auto &[key, value] : dictionary_refs)
			std::cout << key << "  ==  " << value << std::endl;

		dictionary.clear();
	}

	void insert()
	{
		{
			std::map<std::string, std::string> dict;

			dict.insert({ "key1", "value1" });
			dict.insert({ "key2", "value2" });
			dict.insert({ "key3", "value3" });

			for (const auto& entry : dict)
				std::cout << entry.first << " = " << entry.second << std::endl;

			std::cout << "\nAfter update:" << std::endl;

			dict.insert({ "key3", "value3_Updated" });
			for (const auto& entry : dict)
				std::cout << entry.first << " = " << entry.second << std::endl;
		}
		std::cout << "\n------------------------------ Insert or assign --------------------------------------------\n" << std::endl;
		{
			std::map<std::string, std::string> dict;

			dict.insert_or_assign("key1", "value1");
			dict.insert_or_assign("key2", "value2");
			dict.insert_or_assign("key3", "value3");

			for (const auto& entry : dict)
				std::cout << entry.first << " = " << entry.second << std::endl;

			std::cout << "\nAfter update:" << std::endl;

			dict.insert_or_assign("key3", "value3_Updated");
			for (const auto& entry : dict)
				std::cout << entry.first << " = " << entry.second << std::endl;
		}
	}

	void insert_CopyElement_Test()
	{
		std::map<std::string, Integer> dict;
		Integer i1 {1}, i2 {2};
		std::cout << i1 << " " << i2 << std::endl;
		dict.insert({"1", std::move(i1)});
		dict.insert({"2", std::move(i2)});
		std::cout << dict << std::endl;
	}

	void insert_return()
	{
		std::map<std::string, std::string> dict {
				{"key1", "value1"}, {"key2", "value2"}
		};
		auto [iter, ok] = dict.insert(std::pair("key3", "value3"));
		std::cout << dict <<  std::endl;
		iter->second = "Updated";
		std::cout << dict << std::endl;
	}

	void AT_vs_Operator() {
		{
			std::map<int, std::string> dict{ {1, "one"},{2, "two"},{3, "three"} };

			for (const auto&[k, v] : dict)
				std::cout << k << " = " << v << std::endl;

			if (0 == dict[4].compare("four")) {
				std::cout << "We have four at 4" << std::endl;
			}
			else {
				std::cout << "Oppsss!" << std::endl;
			}

			for (const auto&[k, v] : dict)
				std::cout << k << " = " << v << std::endl;
		}

		std::cout << "------------------------------------ Test2 (at) -------------------------------------" << std::endl;

		{
			std::map<int, std::string> dict{ {1, "one"},{2, "two"},{3, "three"} };

			for (const auto&[k, v] : dict)
				std::cout << k << " = " << v << std::endl;

			try {
				const auto& value = dict.at(4);
				if (0 == value.compare("four")) 
					std::cout << "We have four at 4" << std::endl;
			}
			catch (...) {
				std::cout << "Oppsss!" << std::endl;
			}

			for (const auto&[k, v] : dict)
				std::cout << k << " = " << v << std::endl;
		}
	}

	void insert_vs_operator()
	{
		std::cout << "\n--------------------------------- Insert() --------------------------\n" << std::endl;
		{
			std::map<std::string, Integer> dict;
			dict.insert({ "key1", 1 });
		}
		std::cout << "\n--------------------------------- Emplace() --------------------------\n" << std::endl;
		{
			std::map<std::string, Integer> dict;
			dict.emplace("key1", 1);
		}
		std::cout << "\n--------------------------------- Operator[]--------------------------\n" << std::endl;
		{
			std::map<std::string, Integer> dict;
			dict["key1"] = 1;
		}
	}

	void insert_result()
	{
		std::map<std::string, std::string> dict;

		dict.insert(std::pair("key1", "value1"));
		dict.insert(std::pair("key2", "value2"));

		if (auto[iter, result] = dict.insert(std::pair("key3", "value3")); true == result) {
			std::cout << "Add 'key3' Successed " << std::endl;
		} else {
			std::cout << "Add 'key3' Failed " << std::endl;
		}

		if (auto[iter, result] = dict.insert(std::pair("key3", "value3")); true == result) {
			std::cout << "Add 'key3' Successed " << std::endl;
		}
		else {
			std::cout << "Add 'key3' Failed " << std::endl;
		}
	}

	void try_emplace_test() {
		using namespace std::literals;
		std::map<std::string, std::string> dictionary;

		dictionary.try_emplace("Key1", "Value1");
		dictionary.try_emplace("Key2", "Value2");
		dictionary.try_emplace("Key3", "Value3");

		print_map_info(dictionary, ">>> Before:");

		dictionary.try_emplace("Key3", "Value3New");

		print_map_info(dictionary, "\nAfter (dictionary.try_emplace(\"Key3\", \"Value3New\")):");

		dictionary.try_emplace("Key4", "Value3New");

		print_map_info(dictionary, "\nAfter (dictionary.try_emplace(\"Key4\", \"Value3New\")):");
	}

	void try_emplace_test_2() {
		std::map<std::string, Integer> intMap;

		intMap.try_emplace("Key1", 11);
		intMap.try_emplace("Key2", 22);
		intMap.try_emplace("Key3", 33);

		std::cout << "      Object 'Integer(44)' shall not be constructed" << std::endl;
		intMap.try_emplace("Key3", 44);

		Integer int5(55);
		std::cout << "      Object 'Integer(55)' WILL be constructed. But will not be inserted" << std::endl;
		intMap.try_emplace("Key3", int5);

		std::cout << "      Object 'Integer(66)' WILL be constructed and WILL be inserted" << std::endl;
		intMap.try_emplace("Key6", 66);

		print_map(intMap);
	}

	void try_emplace_test_3()
	{
		std::map<std::string, Integer> dictionary;
        [[maybe_unused]]
		auto [iterator1, succeed1] = dictionary.try_emplace("Key1", 111);

		std::cout << ">>> Using 'try_emplace' to add an element Integer(777) with existing key 'Key1' <<<" << std::endl;

        [[maybe_unused]]
		auto [iterator2, succeed2] = dictionary.try_emplace("Key1", 777);

        [[maybe_unused]]
		auto[iterator3, succeed3] = dictionary.try_emplace("Key2", 222);

		assert(succeed1);
		assert(!succeed2);
		assert(succeed3);

		for (auto&&[key, value] : dictionary)
			std::cout << key << ": " << value << "\n";
	}


    void TryEmplace_vs_Emplace()
    {
        std::map<std::string, std::string> m;
        m["Hello"] = "World";
        std::cout << m["Hello"] << std::endl;

        std::string s1 = "C++";
        m.emplace("Hello", std::move(s1));
        std::cout << s1 << "            <------ 's' is empty! Its has been moved!!\n";
        std::cout << m["Hello"] << std::endl;

		std::string s2 = "C++";
        m.try_emplace("Hello", std::move(s2));
        std::cout << s2 << "         <------ 's' is empty! Its has NOT been moved!!\n";
        std::cout << m["Hello"] << std::endl;
    }


	void try_emplace_test_LAMBDA()
	{


#if 0

		std::map<std::string, Integer> dictionary;
		auto [iterator1, succeed1] = dictionary.try_emplace("Key1", [] {
			std::cout << "Lambda(111) called\n";
			return 111;
		});
#endif



		/*

		std::cout << ">>> Using 'try_emplace' to add an element Integer(222) with existing key 'Key1' <<<" << std::endl;

		auto [iterator2, succeed2] = dictionary.try_emplace("Key1", [] {
			std::cout << "Lambda(222) called\n";
			return 222; 
		}());
		auto [iterator3, succeed3] = dictionary.try_emplace("Key2", [] {
			std::cout << "Lambda(333) called\n";
			return 333;
		}());

		assert(succeed1);
		assert(!succeed2);
		assert(succeed3);

		for (auto&& [key, value] : dictionary)
			std::cout << key << ": " << value << "\n";
		*/
	}


	int Try_emplace_vs_Emplace() {
		std::map<std::string, std::string> m;
		m["Hello"] = "World";

		std::string s = "C++";
		m.emplace("Hello", std::move(s));

		// what happens with the string 's'?
		std::cout << "s = " << s << '\n';
		std::cout << m["Hello"] << "\n\n\n";

		s = "C++";
		m.try_emplace("Hello", std::move(s));
		std::cout << s << '\n';
		std::cout << m["Hello"] << '\n';
        return  0;
	}

    /**
     * insert_or_assign() vs operator[]
     * Result: SAME! in this example
    **/
	void insert_or_assign()
    {
        {
            std::map<std::string, Integer> dictionary;

            dictionary.insert_or_assign("Key1", 111);
            dictionary.insert_or_assign("Key2", 222);

            std::cout << "Before:\n" << dictionary << std::endl;

            std::cout << "Modifying the Key2 entry: insert_or_assign()" << std::endl;
            dictionary.insert_or_assign("Key2", 222222);

            std::cout << "\nAfter:\n" << dictionary << std::endl;
        }

        std::cout << "---------------------- operator[]------------------------\n";

        {
            std::map<std::string, Integer> dictionary;

            dictionary.insert_or_assign("Key1", 111);
            dictionary.insert_or_assign("Key2", 222);

            std::cout << "Before:\n" << dictionary << std::endl;

            std::cout << "Modifying the Key2 entry: operator[]" << std::endl;
            dictionary["Key2"] = 222222;

            std::cout << "\nAfter:\n" << dictionary << std::endl;
        }
	}

    /**
     * insert_or_assign() vs operator[] in case of missing element
     * Result:
     *      insert_or_assign better()
     *      with operator[]  --> additional 'Copy assignment operator()' called
    **/
    void insert_or_assign__missing_key()
    {
        {
            std::map<std::string, Integer> dictionary;

            dictionary.insert_or_assign("Key1", 111);

            std::cout << "Before:\n" << dictionary << std::endl;

            std::cout << "Adding the Key2 entry: insert_or_assign()" << std::endl;
            dictionary.insert_or_assign("Key2", 222222);

            std::cout << "\nAfter:\n" << dictionary << std::endl;
        }

        std::cout << "---------------------- operator[]------------------------\n";

        {
            std::map<std::string, Integer> dictionary;

            dictionary.insert_or_assign("Key1", 111);

            std::cout << "Before:\n" << dictionary << std::endl;

            std::cout << "Adding the Key2 entry: operator[]" << std::endl;
            dictionary["Key2"] = 222222;

            std::cout << "\nAfter:\n" << dictionary << std::endl;
        }
    }
	void emplace_hint_test1()
	{
		std::map<char, int> mymap;
		auto it = mymap.end();

		it = mymap.emplace_hint(it, 'b', 10);
		mymap.emplace_hint(it, 'a', 12);
		mymap.emplace_hint(mymap.end(), 'c', 14);

		std::cout << "mymap contains:";
		for (auto& x : mymap)
			std::cout << " [" << x.first << ':' << x.second << ']';
		std::cout << '\n';
	}

	void test_loops() {
		std::map<std::string, std::string> dict;

		dict.insert(std::pair("key1", "value1"));
		dict.insert(std::pair("key2", "value3"));
		dict.insert(std::pair("key3", "value3"));

		std::cout << "For 1:" << std::endl;
		for (const std::pair<std::string, std::string>& entry : dict) {
			std::cout << entry.first << "  ==  " << entry.second << std::endl;
		}

		std::cout << "\nFor 2:" << std::endl;
		std::map<std::string, std::string>::const_iterator iter = dict.begin();
		for (; iter != dict.end(); ++iter) {
			std::cout << iter->first << "  ==  " << iter->second << std::endl;
		}

		std::cout << "\nFor 3:" << std::endl;
		for (auto &[key, value] : dict) {
			std::cout << key << "  ==  " << value << std::endl;
		}

		std::cout << "\nFor 4:" << std::endl;
		for (auto& x : dict) {
			std::cout << x.first << "  ==  " << x.second << std::endl;
		}
	}

    void test_loops_backwards()
    {
        std::map<int, std::string> dict{
                {1, "I"},
                {2, "II"},
                {3, "III"},
        };

        for (auto iter = std::prev(dict.end()); ; --iter) {
            std::cout << iter->first << " = " << iter->second << std::endl;
            if (iter == dict.begin()) break;
        }

        std::cout << std::endl;

        for (auto iter = dict.rbegin(); iter != dict.rend(); ++iter)
            std::cout << iter->first << " = " << iter->second << std::endl;

        std::cout << std::endl;

        for (auto& [k, v] : std::ranges::reverse_view(dict))
            std::cout << k << " = " << v << std::endl;
    }

	void Rbegin_Rend_Test() {
		std::map<char, int> test_map = { {'a',100}, {'b',200}, {'c',300}, {'d',400}, {'e',500} };
		std::cout << "Original map content:" << std::endl;
		for (const auto& x : test_map)
			std::cout << x.first << " => " << x.second << std::endl;

		auto entry = test_map.rbegin();
		std::cout << "\nrbegin() points to: {" << entry->first << "," << entry->second << "}" << std::endl;

		std::cout << "\nReversed map content:" << std::endl;
		for (auto r_iter = test_map.rbegin(); r_iter != test_map.rend(); ++r_iter)
			std::cout << r_iter->first << " => " << r_iter->second << std::endl;
	}

	void KeyCompare() {
		std::map<char, int> map = { {'a',100}, {'b',200}, {'c',300} };
		std::map<char, int>::key_compare kcompare = map.key_comp();

		std::cout << "mymap contains:" << std::endl;

		// key value of last element
		char highest = map.rbegin()->first;

		std::map<char, int>::iterator iter = map.begin();
		do {
			std::cout << iter->first << " => " << iter->second << std::endl;
		} while (kcompare((*iter++).first, highest));

		std::cout << std::endl;
	}

	void KeyValue() {
		std::map<char, int> mymap;

		mymap['x'] = 2001;
		mymap['y'] = 1002;
		mymap['z'] = 3003;

		std::cout << "mymap contains:" << std::endl;
		std::pair<char, int> highest = *mymap.rbegin();          // last element
		std::map<char, int>::iterator it = mymap.begin();
		do {
			std::cout << it->first << " => " << it->second << std::endl;
		} while (mymap.value_comp()(*it++, highest));
	}

	void ExtractValue() {
		std::map<std::string, std::string> dictionary;

		dictionary.try_emplace("Key1", "Value1");
		dictionary.try_emplace("Key2", "Value2");
		dictionary.try_emplace("Key3", "Value3");

		auto node = dictionary.extract("Key32");
		if (node.empty()) {
			std::cout << "Failed to extract node by key 'Key32'"<< std::endl;
		} else {
			std::cout << "Node extracted: " << node.key() << std::endl;
		}

		node = dictionary.extract("Key2");
		if (node.empty()) {
			std::cout << "Failed to extract node by key 'Key32'" << std::endl;
		} else {
			std::cout << "Node extracted: {" << node.key() << ", " << node.mapped()  << "}" << std::endl;
		}
		print_map_info(dictionary, "\nAfter:");
	
	}

	void UpdateValue() {
		std::map<std::string, std::string> dictionary;

		dictionary.try_emplace("Key1", "Value1");
		dictionary.try_emplace("Key2", "Value2");
		dictionary.try_emplace("Key3", "Value3");

		auto iter = dictionary.find("Key2");
		iter->second = "Value2_updated";

		print_map_info(dictionary, "\nAfter:");
	}

	void LowerBound(){
		{
			const std::map<int, std::string> map {
                    {1, "I"},
                    {2, "II"},
                    //{3, "III"},
                    {4, "IV"},
                    {5, "V"},
                    {6, "VI"},
                    {7, "VII"},
            };

			auto lower = map.lower_bound(3);
			std::cout << "Lower bound (3): [" << lower->first << ", " << lower->second << "]" << std::endl;

			auto upper = map.upper_bound(3);
			std::cout << "Upper bound (3): [" << upper->first << ", " << upper->second << "]" << std::endl;
		}
		std::cout << "-------------------------------- Test 2: --------------------------------\n" << std::endl;
		{
			std::map<int, std::string> map = {
                    {1, "one"},
                    {2, "two"},
                    {3, "three"},
                    {4, "four"},
                    {5, "five"}
            };

			auto lower = map.lower_bound(3);
			std::cout << "Lower bound (3): [" << lower->first << "," << lower->second << "]" << std::endl;

			auto upper = map.upper_bound(3);
			std::cout << "Upper bound (3): [" << upper->first << "," << upper->second << "]" << std::endl;
		}
	}

	void UpperBound() {
		{
			std::map<int, std::string> map = {
                    {1, "one"},
                    //{2, "two"},
                    //{3, "three"},
                    //{4, "four"},
                    {5, "five"} };

			auto upper = map.upper_bound(2);
			std::cout << "Upper bound (2): [" << upper->first << "," << upper->second << "]" << std::endl;

			upper = map.upper_bound(3);
			std::cout << "Upper bound (3): [" << upper->first << "," << upper->second << "]" << std::endl;

			/*for (auto iter = map.begin(); upper != iter; ++iter) {
				std::cout << "[" << iter->first << ", " << iter->second << "]" << std::endl;
			}*/
		}
		std::cout << "-------------------------------- Test 2:" << std::endl;
		{
			std::map<int, std::string> map = { {1, "one"}, {2, "two"}, /*{3, "three"},*/ {4, "four"}, {5, "five"} };

			auto upper = map.upper_bound(2);
			std::cout << "Upper bound (2): [" << upper->first << "," << upper->second << "]" << std::endl;

			upper = map.upper_bound(3);
			std::cout << "Upper bound (3): [" << upper->first << "," << upper->second << "]" << std::endl;

			/*for (auto iter = map.begin(); upper != iter; ++iter) {
				std::cout << "[" << iter->first << ", " << iter->second << "]" << std::endl;
			}*/
		}
	}

	void Modify_Value() {
		std::map<std::string, std::string> dictionary;
		auto it = dictionary.emplace("Key1", "Empty");

		std::cout << it.first->first << "   " << it.first->second << std::endl;

		dictionary.emplace("Key2", "Empty");
		dictionary.emplace("Key3", "Empty");

		if (auto iter = dictionary.find("Key1"); dictionary.end() != iter)
			iter->second.assign("###########################");

		for (const auto&[k, v] : dictionary) {
			std::cout << k << "   " << v << std::endl;
		}
	}

	void Custom_Comparator() {
		{
			const std::map<int, char, decltype([](const auto& a, const auto& b) { return a > b; }) > dict{
				{2, 't'},
				{5, 'w'},
				{4, 'e'},
				{6, 'q'},
				{3, 'r'},
				{1, 'y'}
			};

			std::for_each(dict.cbegin(), dict.cend(), [](const auto& entry) {
				std::cout << entry.first << " = " << entry.second << std::endl;
				});
		}

		std::cout << " ============================================= Test2 ==================================\n";

		{
			const std::map<int, char, decltype([](const auto& a, const auto& b) { return a < b; }) > dict{
				{2, 'w'},
				{5, 't'},
				{4, 'r'},
				{6, 'y'},
				{3, 'e'},
				{1, 'q'}
			};

			std::for_each(dict.cbegin(), dict.cend(), [](const auto& entry) {
				std::cout << entry.first << " = " << entry.second << std::endl;
			});
		}
	}



	void Custom_Comparator_2() {
		auto greater = [](auto x, auto y) -> bool {
			return x < y;
		};
	
		std::map<int, char, decltype(greater)> dict {
			{2, 'w'},
			{5, 't'},
			{4, 'r'},
			{6, 'y'},
			{3, 'e'},
			{1, 'q'}
		};

		std::for_each(dict.cbegin(), dict.cend(), [](const auto& entry) {
			std::cout << entry.first << " = " << entry.second << std::endl;
		});
	}

	void Sorted_Map() {
		std::map<std::string, int, std::less<std::string>> dict;

		{
			dict.emplace("Max", 13);
			dict.emplace("John", 5);
			dict.emplace("Anna", 51);
		}

		for (const auto&[k, v] : dict) {
			std::cout << k << " = " << v << std::endl;
		}
	}

	void Sorted_Map_2() {
		{
			std::map<int, char, std::less<int>> dict;

			dict[3] = 'a';
			dict[2] = 'b';
			dict[1] = 'c';

			for (const auto&[k, v] : dict) {
				std::cout << k << " = " << v << std::endl;
			}
		}
		{
			std::map<int, char, std::greater<int>> dict;

			dict[3] = 'a';
			dict[2] = 'b';
			dict[1] = 'c';

			for (const auto&[k, v] : dict) {
				std::cout << k << " = " << v << std::endl;
			}
		}
	}

	void Erase_ByVal() {
		std::map<std::string, int> mymap;

		mymap.insert({ "One", 1 });
		mymap.insert({ "Two", 2 });
		mymap.insert({ "Three", 3 });

		auto result = mymap.erase("One");
		std::cout << result << std::endl;

		for (const auto&[k, v] : mymap)
			std::cout << k << " = " << v << std::endl;
	}

	void Erase() {
		std::map<char, int> mymap;
		std::map<char, int>::iterator it;

		// insert some values:
		mymap['a'] = 10;
		mymap['b'] = 20;
		mymap['c'] = 30;
		mymap['d'] = 40;
		mymap['e'] = 50;
		mymap['f'] = 60;

		for (const auto&[k, v] : mymap) {
			std::cout << k << " = " << v << std::endl;
		}

		std::cout << "\nAfter\n"<< std::endl;

		mymap.erase(mymap.begin(), mymap.find('d'));
		for (const auto&[k, v] : mymap) {
			std::cout << k << " = " << v << std::endl;
		}
	}


    void Erase_Interval() {
        std::map<int, char> map {
            {0, 'a'}, {1, 'b'}, {2, 'v'}, {3, 'd'}, {4, 'e'},
            {5, 'f'}, {6, 'g'}, {7, 'h'}, {8, 'k'}, {9, 'z'},
        };

        for (const auto&[k, v] : map)
            std::cout << k << " = " << v << std::endl;

        {
            auto from = map.begin();
            std::advance(from, 2);

            auto end = map.begin();
            std::advance(end, 6);

            map.erase(from, end);
        }

        std::cout << "--------------------------------------------------\n";

        for (const auto&[k, v] : map)
            std::cout << k << " = " << v << std::endl;

    }
};

namespace Map::Map_Of_Vectors {

	template<typename K, typename V>
	void print(const std::map<K, std::vector<V>>& map) {
		for (const auto&[id, vect] : map) {
			std::cout << id << " --> [ ";
			for (const V& v : vect)
				std::cout << v << " ";
			std::cout << "]" << std::endl;
		}
	}

	void AddRecord() {
		std::map<int, std::vector<Integer>> consumers;
	
		auto [records, ok] = consumers.try_emplace(1);

		records->second.reserve(10);
		records->second.emplace_back(1);
		records->second.emplace_back(2);

		print(consumers);

		/*

		auto result1 = consumers.try_emplace(1, std::vector<Integer>());
		std::cout << std::boolalpha << result1.second << std::endl;
		result.first->second.push_back(3);


		print(consumers);
		*/
	}
}

namespace Map::FunnyBugs {

	void Print_Map_Element() {
		std::map<std::string, int> dict;
		dict["hey"] = 123;
		std::cout << dict["hye"] << std::endl;
	}
}

namespace Map::Test {

	void ForEach_Test() {
		const std::map<int, std::string> dict {
			{1, "one"},
			{2, "two"},
			{3, "three"},
			{4, "four"},
			{5, "five"},
		};

		std::for_each(dict.begin(), dict.end(), [](auto e) {
			std::cout << e.first << " = " << e.second << std::endl;
		});

		auto result = std::find_if(dict.begin(), dict.end(), [](const auto& entry) {
			return 0 == entry.second.compare("three");
		});
		std::cout << "Result: " << result->first << std::endl;
	}

    void Iterators_Invalidation() {
        std::map<int, std::string> dict{};

        dict.emplace(1, "I");
        auto [iter, ok] = dict.emplace(5, "V");

        //std::cout << iter->second << std::endl;

        dict.emplace(3, "III");
        dict.emplace(4, "IV");
        dict.emplace(2, "II");

        //std::cout << iter->second << std::endl;

        dict.emplace(6, "VI");
        dict.emplace(7, "VII");

        dict.erase(dict.find(4));

        //std::cout << iter->second << std::endl;

        for (auto it = dict.begin(); dict.end() != it; ++it) {
            std::cout << it->first << " -> " << it->second << "   ";
            if (it == iter) {
                std::cout << " OK ";
            }
            std::cout << std::endl;
        }

    }
}

void Map::TEST_ALL()
{
	// test_loops();
	test_loops_backwards();

    // emplace_duplicate_entry();
    // try_emplace_duplicate_entry();

	// emplace_test();
	// emplace_test1();
	// emplace_return();
	// emplace_vs_insert();
	// try_emplace_vs_emplace();
	// mplace_test_2_ReferenceWrapper();

	// Sorted_Map();
	// Sorted_Map_2();

	// Custom_Comparator();
	// Custom_Comparator_2();

	// insert();
	// insert_return();
	// insert_CopyElement_Test();
	// AT_vs_Operator();
	// insert_vs_operator();
	// insert_result();

	// insert_or_assign();
    // insert_or_assign__missing_key();

	// try_emplace_test();
	// try_emplace_test_2();
	// try_emplace_test_3();
	// try_emplace_test_LAMBDA();
	// Try_emplace_vs_Emplace();
    // TryEmplace_vs_Emplace();

	// emplace_hint_test1();
	// Rbegin_Rend_Test();

	// KeyCompare();
	// KeyValue();

	// Erase_ByVal();
	// Erase();
    // Erase_Interval();

	// UpdateValue();
	// ExtractValue();

	// LowerBound();
	// UpperBound();

	// Modify_Value();

	// FunnyBugs::Print_Map_Element();

	// Map_Of_Vectors::AddRecord();

	// Test::ForEach_Test();
	// Test::Iterators_Invalidation();

}