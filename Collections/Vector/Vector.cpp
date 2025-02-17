/**============================================================================
Name        : Vector.cpp
Created on  : 15.04.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ STL Vector container testing
============================================================================**/

#include "Vector.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <string_view>
#include <cassert>
#include <future>
#include <algorithm>
#include <chrono>
#include "../Helpers/Integer.h"


/** Vector_Testing namespace: **/
namespace Vector {

	/** President struct: **/
	class President {
	public:
		std::string name;
		std::string country;
		int year;

	public:
		President(std::string&& p_name, std::string&& p_country, int p_year) : name(std::move(p_name)),
			country(std::move(p_country)),
			year(p_year) {
			std::cout << "I am being constructed (" << name << ")" << std::endl;
		}
		President(President&& other) : name(std::move(other.name)),
			country(std::move(other.country)),
			year(other.year) {
			std::cout << "I am being moved (" << name << ")" << std::endl;
		}

	public:
		President& operator=(const President& other) = default;
	};

	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_vector = [](const auto& vector) {
		std::for_each(vector.begin(), vector.end(), printer_coma);
	};

	auto print_vector_ex = [](const auto& vector,
							  std::string_view text,
							  std::string_view postfix = "\n") {
		std::cout << text;
		print_vector(vector);
		std::cout << postfix << std::endl;
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
		for (const T& entry : vect)
			stream << entry << ' ';
		return stream;
	}

	void print_vector_of_Integer(const std::vector<Integer>& data) {
		for (const Integer& i : data)
			std::cout << i.getValue() << " ";
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////


	void Constructors_Tests()
	{
		std::vector<std::string> words{ "First", "Second", "Third" };
		print_vector_ex(words, "Contructor 1 test:\n");

		std::vector<std::string> words2(words.begin(), words.end());
		print_vector_ex(words2, "Contructor 2 test:\n");

		std::vector<std::string> words3(words2);
		print_vector_ex(words3, "Contructor 3 test:\n");

		std::vector<std::string> words4(5, "Mo");
		print_vector_ex(words4, "Contructor 4 test:\n");


		std::vector<int> ints(10);
		std::cout << "std::vector<int> ints(10). Size = " << ints.size() << std::endl;
		print_vector(ints);


		std::cout << "\n\n------------------std::vector<int>(5, 222) ----------------" << std::endl;
		std::vector<int> ints1(5, 222);
		print_vector(ints1);
	}

	void PopBack() {
		std::vector<std::string> words{ "One", "Two", "Three", "Four", "Five" };
		print_vector_ex(words, "words ");

		std::cout << ">>  words.pop_back()." << std::endl;
		words.pop_back();
		print_vector_ex(words, "words ");

		std::cout << ">>  words.pop_back()." << std::endl;
		words.pop_back();
		print_vector_ex(words, "words ");
	}

	void Back_Test() {
		std::vector<int> myvector(1, 10);
		while (myvector.back() != 0) {
			std::cout << "myvector.back() = " << myvector.back() << std::endl;
			myvector.push_back(myvector.back() - 1);
		}
		print_vector_ex(myvector, "myvector contains: ", "");
	}

	void ShrinkToFit_Test() {
		std::vector<int> ints;
		ints.reserve(100);
		std::cout << "Size: " << ints.size() << ", Capacity: " << ints.capacity() << std::endl;

		for (int i = 0; i < 50; i++)
			ints.emplace_back(i);

		std::cout << "Size: " << ints.size() << ", Capacity: " << ints.capacity() << std::endl;

		ints.shrink_to_fit();
		std::cout << "Size: " << ints.size() << ", Capacity: " << ints.capacity() << std::endl;
	}

	void Assign_Test() {
		const int data[] = { 1,2,3,4,5,6,7,8,9,10 };
		constexpr size_t length = sizeof(data) / sizeof(data[0]);
		std::vector<int> vect;
		vect.reserve(100);

		std::cout << "\n--------------------------  Test1 ------------------------ " << std::endl; {
			vect.assign(5, 3);
			print_vector_ex(vect, "vect: ");
		}

		std::cout << "\n--------------------------  Test2 ------------------------ " << std::endl; {
			vect.assign(data, data + length);
			print_vector_ex(vect, "vect: ");
		}

		std::cout << "\n--------------------------  Test2 ------------------------ " << std::endl; {
			std::vector<int> vect2;
			vect2.assign(vect.begin(), vect.end());
			print_vector_ex(vect2, "vect2: ");
		}

		std::cout << "\n--------------------------  Test4 ------------------------ " << std::endl; {
			vect.assign({ 5,4,3,2,1 });
			print_vector_ex(vect, "vect: ");
			vect.clear();
		}
	}

	void Resize_Capacity_Tests()
	{
		std::vector<std::string> words{ "One", "Two", "Three" };
		print_vector_ex(words, "words:\n");

		std::cout << "Size: " << words.size() << ". capacity = " << words.capacity() << std::endl;

		words.reserve(10);
		std::cout << "words.reserve(10)" << std::endl;

		std::cout << "Size: " << words.size() << ". capacity = " << words.capacity() << std::endl;

		words.emplace_back("Four");
		words.emplace_back("Five");

		std::cout << "Size: " << words.size() << ". capacity = " << words.capacity() << std::endl;
	}

	void Resize_Capacity_Tests_2()
	{
		std::vector<std::string> words1{ "Value1", "Value2", "Value3", "Value4" , "Value5" };
		std::vector<std::string> words2{ "Value6", "Value7" };

		std::cout << "words1: Size: " << words1.size() << ", Space left: " << words1.capacity() - words1.size() << std::endl;
		words1.insert(words1.end(), words2.begin(), words2.end());

		std::cout << "words1: Size: " << words1.size() << ", Space left: " << words1.capacity() - words1.size() << std::endl;
	}

    void Clear_Capacity_Tests()
    {
        std::vector<int> numbers {1,2,3,4,5,6,7,8,9};
        std::cout << "Size: " << numbers.size() << ". capacity = " << numbers.capacity() << std::endl;

        numbers.clear();
        std::cout << "Size: " << numbers.size() << ". capacity = " << numbers.capacity() << std::endl;
    }

	void Allocate_Memory_Tests()
	{
		std::vector<Integer> ints;
		for (int v : {1,2,3,4,5})
			ints.emplace_back(v);
	}


	void Insert()
	{
		const int data[] = {1,2,3,4,5,6,7,8,9,10};
		constexpr size_t length = sizeof(data) / sizeof(data[0]);
		std::vector<int> vect;
		vect.reserve(100);

		std::cout << "--------------------------  Test1 ------------------------ " << std::endl; {
			vect.insert(vect.begin(), data, data + length);
			print_vector_ex(vect, "vect: ");
		}

		std::cout << "\n--------------------------  Test2 ------------------------ " << std::endl; {
			vect.insert(vect.begin(), data, data + length);
			print_vector_ex(vect, "vect: ");
			vect.clear();
		}

		std::cout << "\n--------------------------  Test3 ------------------------ " << std::endl; {
			vect.insert(vect.begin(), {5,4,3,2,1});
			print_vector_ex(vect, "vect: ");
			vect.clear();
		}

		std::cout << "\n--------------------------  Test4 ------------------------ " << std::endl; {
			for (int i = 0; i < 10; i++)
				vect.insert(vect.begin() + i, i);
			print_vector_ex(vect, "vect: ");
			vect.clear();
		}

		std::cout << "\n--------------------------  Test5 ------------------------ " << std::endl; {
			vect.insert(vect.begin(), 3, 1);
			print_vector_ex(vect, "vect: ");
			vect.clear();
		}
	}

	void Insert_At_TheEnd()
	{
		std::vector<int> v1 {1,2,3,4,5};
		std::vector<int> v2{ 6,7,8,9,10 };

		v1.insert(v1.end(), v2.begin(), v2.end());

		std::cout << v1 << std::endl;
	}

	void Create_and_Print() {
		std::vector<int> v = { 7, 5, 16, 8 };

		std::for_each(std::begin(v), std::end(v), printer_coma);
		std::cout << std::endl;

		v.push_back(25);
		v.push_back(13);

		std::for_each(v.begin(), v.end(), printer_coma);
	}

	void Erase_Tests()
	{
		std::vector<int> numbers = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		print_vector_ex(numbers, "");

		numbers.erase(numbers.begin() + 5);
		print_vector_ex(numbers, "");

		numbers.erase(numbers.begin() + 2, numbers.begin() + 5);
		print_vector_ex(numbers, "");
	}

    void Erase_Tests_2()
    {
        std::vector<int> numbers = { 1,2,3,4, -1, -2, -3,5,6,7};
        std::cout << numbers << std::endl;

        const auto  it = std::remove_if(numbers.begin(), numbers.end(), [](const auto v) {
            return 0 > v;
        });

        for (auto iter = numbers.begin(); iter != it; ++iter)
            std::cout << *iter << ' ';
        std::cout << "[ ";
        for (auto iter = it; iter != numbers.end(); ++iter)
            std::cout << *iter << ' ';
        std::cout << "]\n";
    }

    void Erase_IF()
    {
        std::vector<int> numbers = { 1,2,3,4, -1, -2, -3,5,6,7};
        std::cout << numbers << std::endl;


        std::erase_if(numbers, [](const auto v) {
            return 0 > v;
        });
        std::cout << numbers << std::endl;

    }

	void Erase_Tests_Loop()
	{
		std::vector<Integer> ints = {11,22,33,44,55};
		print_vector_ex(ints, "\nBefore: ");

		for (std::vector<Integer>::iterator iter = ints.begin(); ints.end() != iter; ) {
			if (iter->getValue() == 33)
				ints.erase(iter++);
			else ++iter;
		}

		print_vector_ex(ints, "\nAfter: ");
	}

	void Emplace_Back_SimpleTest()
	{
		std::vector<std::string> texts(10);

		std::cout << "texts size = " << texts.size() << std::endl;

		texts.emplace_back("Text1");
		texts.emplace_back("Text2");
		texts.emplace_back("Text3");

		std::cout << "texts size = " << texts.size() << std::endl;

		for (const std::string& T : texts) {
			std::cout << T << std::endl;
		}
	}

	void Emplace_Back_SimpleTest1()
	{
		std::vector<std::string> strings;

		strings.emplace_back(4, 'A');
		strings.emplace_back(6, 'B');
		strings.emplace_back(8, 'C');

		print_vector_ex(strings, "");
	}

	void Emplace_Back_Tests()
	{
		std::vector<President> elections;
		std::cout << "emplace_back:\n";
		elections.emplace_back("Nelson Mandela", "South Africa", 1994);

		std::vector<President> reElections;
		std::cout << "\npush_back:\n";
		reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));

		std::cout << "\nContents:\n";
		for (President const& president : elections) {
			std::cout << president.name << " was elected president of " << president.country << " in " << president.year << ".\n";
		}
		for (President const& president : reElections) {
			std::cout << president.name << " was re-elected president of " << president.country << " in " << president.year << ".\n";
		}
	}

	void Emplace_Back_GOOD_EXAMPLE() {
		std::vector<std::string> text;

		std::cout << "Text lines size: " << text.size() << std::endl;

		auto& line = text.emplace_back("Hello");

		std::cout << text.back() << std::endl;

		line += " __ ";
		std::cout << text.back() << std::endl;

		line += "World!";
		std::cout << text.back() << std::endl;

	}

	void Performance_PushBack_vs_EmplaceBack()
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();

		auto durtion = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		std::cout << durtion << std::endl;
	}

	void Compare_Vectors()
	{
		std::vector<std::string> vector1{ "One", "Two", "Three", "Four" };
		std::vector<std::string> vector2{ "One", "Two", "Three", "Four", "Five" };
		std::vector<std::string> vector3{ "One", "Two", "Three", "Five", "Four" };
		std::vector<std::string> vector4{ "One", "Two", "Three", "Four" };

		print_vector_ex(vector1, "vector1: ", "");
		print_vector_ex(vector2, "vector2: ", "");
		print_vector_ex(vector3, "vector3: ", "");
		print_vector_ex(vector4, "vector4: ");

		if (vector1 == vector2) {
			std::cout << "vector1 == vector2" << std::endl;
		}
		else {
			std::cout << "vector1 != vector2" << std::endl;
		}

		if (vector2 == vector3) {
			std::cout << "vector2 == vector3" << std::endl;
		}
		else {
			std::cout << "vector2 != vector3" << std::endl;
		}

		if (vector1 == vector3) {
			std::cout << "vector1 == vector3" << std::endl;
		}
		else {
			std::cout << "vector1 != vector3" << std::endl;
		}

		if (vector1 == vector4) {
			std::cout << "vector1 == vector4" << std::endl;
		}
		else {
			std::cout << "vector1 != vector4" << std::endl;
		}
	}

	void Sort_Test1()
	{
		std::vector<int> ints{ 1, 22, 34, -1, 65, 3, 6 };
		print_vector_ex(ints, "Vector containt: ");

		{	/** stable_sort() test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::stable_sort(ints.begin(), ints.end()):" << std::endl;
			std::stable_sort(scoped_vector.begin(), scoped_vector.end());
			print_vector_ex(scoped_vector, "Vector containt: ");
		}

		{	/** sort() test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::sort(ints.begin(), ints.end()):" << std::endl;
			std::sort(scoped_vector.begin(), scoped_vector.end());
			print_vector_ex(scoped_vector, "Vector containt: ");
		}

		{	/** sort() with std::greater<int>() test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::sort(ints.begin(), ints.end(), std::greater<int>()):" << std::endl;
			std::sort(scoped_vector.begin(), scoped_vector.end(), std::greater<int>());
			print_vector_ex(scoped_vector, "Vector containt: ");
		}


		{	/** sort() with |custom_less_comparator| test. **/
			const auto custom_less_comparator = [](int a, int b)-> bool {
				return a < b;
			};

			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::sort(ints.begin(), ints.end(), custom_less_comparator):" << std::endl;
			std::sort(scoped_vector.begin(), scoped_vector.end(), custom_less_comparator);
			print_vector_ex(scoped_vector, "Vector containt: ");
		}

		{	/** sort() with |custom_greather_comparator| test. **/
			const auto custom_greather_comparator = [](int a, int b)-> bool {
				return a > b;
			};

			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::sort(ints.begin(), ints.end(), custom_greather_comparator):" << std::endl;
			std::sort(scoped_vector.begin(), scoped_vector.end(), custom_greather_comparator);
			print_vector_ex(scoped_vector, "Vector containt: ");
		}


		{	/** sort() with LAMBDA test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::sort(ints.begin(), ints.end(), lambda (Greather)):" << std::endl;
			std::sort(scoped_vector.begin(), scoped_vector.end(), [](int a, int b)-> bool { return a > b; });
			print_vector_ex(scoped_vector, "Vector containt: ");
		}
	}

    void Sort_Test_Pair() {
        using Author = std::pair<std::string, std::string>;
        std::vector<Author> authors = {
                {"Fyodor", "Dostoevsky"},
                {"Sylvia", "Plath"},
                {"Douglas", "Zinger"},
                {"Vladimir", "Nabokov"},
                {"Douglas", "Hofstadter"},
        };

        // Sort by first name, then by last name.
        std::sort(authors.begin(), authors.end(), [](auto&& a, auto&& b) {
            return std::tie(a.first, a.second) < std::tie(b.first, b.second);
        });

        for (const auto& [f, s]: authors)
            std::cout << f << " " << s << std::endl;
    }

	void Partial_Sort_Test()
	{
		std::vector<int> ints{ 1, 22, 34, -1, 65, 3, 6 };
		print_vector_ex(ints, "Vector containt: ");

		{	/** partial_sort() test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::partial_sort(scoped_vector.begin(), scoped_vector.begin() + 3, scoped_vector.end()):" << std::endl;
			std::partial_sort(scoped_vector.begin(), scoped_vector.begin() + 3, scoped_vector.end());
			print_vector_ex(scoped_vector, "Vector containt: ");
		}

		{	/** partial_sort() test. **/
			std::vector<int> scoped_vector(ints);
			std::cout << "<<  std::partial_sort(scoped_vector.begin(), scoped_vector.begin() + 3, scoped_vector.end(), <Lambda-Greater>):" << std::endl;
			std::partial_sort(scoped_vector.begin(), scoped_vector.begin() + 3, scoped_vector.end(), [](int a, int b)-> bool { return a > b; });
			print_vector_ex(scoped_vector, "Vector containt: ");
		}
	}

	void __TESTS__()
    {

        std::vector<std::future<void>> workers;
        std::vector<std::string> lines;
        lines.reserve(10);

        for (int i = 0; i < 3; i++) {
            std::string& str = lines.emplace_back();
            workers.push_back(std::async(std::launch::async, [&] {
                std::cout << "Started" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << &str << std::endl;
            }));
        }

        for (auto &T: workers)
            T.wait();
	}

	void Swap() {
		std::vector<Integer> orig;
		orig.reserve(10);
		for (int i : {1,2,3,4,5,6,7,8,9,10})
			orig.emplace_back(i);

		std::cout << "Orig vector: Size: " << orig.size() << ", Capacity: " << orig.capacity() << std::endl;
		std::cout << "\nDelete some data\n" << std::endl;


		orig.erase(std::next(orig.begin(), 5), orig.end());

		std::cout << "Orig vector: Size: " << orig.size() << ", Capacity: " << orig.capacity() << std::endl;


		std::vector<Integer> dest;
		std::cout << "Dest vector: Size: " << dest.size() << ", Capacity: " << dest.capacity() << std::endl;


		std::cout << "Orig: ["; print_vector_of_Integer(orig); std::cout << "]\n";
		std::cout << "Dest: ["; print_vector_of_Integer(dest); std::cout << "]\n";


		std::cout << "\nSwap\n" << std::endl;
		dest.swap(orig);


		std::cout << "Orig: ["; print_vector_of_Integer(orig); std::cout << "]\n";
		std::cout << "Dest: ["; print_vector_of_Integer(dest); std::cout << "]\n";

		std::cout << "Orig vector: Size: " << orig.size() << ", Capacity: " << orig.capacity() << std::endl;
		std::cout << "Dest vector: Size: " << dest.size() << ", Capacity: " << dest.capacity() << std::endl;

	}

	void KeepAddresses_IncreaseCapacity() {
		std::vector<int> numbers{1,2,3,4,5};

		auto* v3 = &numbers[4];

		for (int i = 0; i < 10; i++) {
			std::cout << numbers.capacity() << ".   v3 = " << v3 << std::endl;
			numbers.push_back(i);
		}
	}

    void KeepAddresses_IncreaseCapacity_2() {
        std::vector<std::string> lines;
        // lines.reserve(10);

        std::string& str = lines.emplace_back("First");
        std::cout << lines.capacity() << std::endl;

        lines.emplace_back("Two");
        lines.emplace_back("Three");
        lines.emplace_back("Four");

        std::cout << lines.capacity() << std::endl;

        str.append("_NEW"); // TODO: Crash here!!!! without lines.reserve(10);

        std::cout << lines.capacity() << std::endl;
        std::cout << lines.front() << std::endl;


    }
}

namespace Vector::BoolVector {

	void CreateBoolVector() {

	}
}


namespace Vector::CustomObjects {

    class Object {
    private:

    public:
        Object() {
            std::cout << "Object()\n";
        }

        Object(const Object& obj) = delete;
        Object& operator=(const Object& obj) = delete;

        Object(Object&&) noexcept {
        }

        Object& operator=(Object&&) noexcept {
            return *this;
        }

        ~Object() {
            std::cout << "~Object()\n";
        }
    };


    void PushCustomOjbectToVector() {

        std::vector<Object> objs {};

        objs.emplace_back();

        // std::vector<Object> objs2 = objs;
    }
}

namespace Vector::ObjectVector {

	class Object {
	private:
		std::string text {};

	public:
		Object(std::string s) noexcept : text(std::move(s)) {
			std::cout << "Object(" << text << ")\n";
		}

		~Object() {
			std::cout << "~Object(" << text << ")\n";
		}

		friend std::ostream& operator<<(std::ostream& stream, const Object& obj);
	};

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.text;
		return stream;
	}



	void CreateVectorOfObject_NoDefaultConstructor() {

#if 0
		std::vector<Object> objs(3);
#endif // 0

		
	}

	void CreateVectorOfObject_NoDefaultConstructor_Hack() {
		constexpr size_t size = 10;
		void* raw_mem = new char[size * sizeof(Object)]; // OR like this  ==>  void* raw_mem = operator new[](size * sizeof(Object));
		Object* pool = static_cast<Object*>(raw_mem);

		for (size_t index = 0; index < size; ++index)
			new (&pool[index]) Object("Value_" + std::to_string(index));

		std::vector<Object> T;
		T.reserve(size);

		memmove(T.data(), raw_mem, size * sizeof(Object));
		for (auto& s : T)
			std::cout << s << std::endl;

		/** Delete memoty allocated: **/
		auto ptr = std::launder(reinterpret_cast<Object*>(pool));
		std::destroy(ptr, ptr + size);

		/** Delete memoty allocated: **/
		delete[] raw_mem; // OR like this  ==>  operator delete[](raw_mem);
	}
}

namespace Vector::Eperiments_And_Tests {

	namespace Network
	{
		using Socket = int;
		constexpr int INVALID_SOCKET{-1};

		/** Client request class. **/
		//  Class that implements a request from a client. Containing the socket handle of 
		//  the specific client  TCP connection and the data itself (in string representation).
		//  Used to pass it from the network request selector thread to the client request 
		//  handler thread
		template<typename T = std::string>
		class Request {
		public:
			/** Client connection socket handle. **/
			Socket socket = INVALID_SOCKET;

			/** Client connection socket handle. **/
			T request{};

		public:
			Request() = default;
			// Request(int s, T r): socket(s), request(std::move(r)) {}

			/** Request constructor. **/
			template<typename... Types>
			Request(int s, Types&&... params) :
				socket(s), request(std::forward<Types>(params)...) {
			}

			// Disable the copy construcor.
			Request(const Request& req) = delete;

			// Disable the copy assignment operator.
			Request& operator=(const Request& req) = delete;

			// Request(Request&& req) noexcept = delete;
			// Request& operator=(Request&& req) noexcept = delete;

			Request(Request&& req) noexcept :
				socket(std::exchange(req.socket, INVALID_SOCKET)),
				request(std::move(req.request)) {
			}

			Request& operator=(Request&& req) noexcept {
				if (this == &req) {
					return *this;
				}
				this->socket = std::exchange(req.socket, INVALID_SOCKET);
				this->request = std::move(req.request);
				return *this;
			}
		};
	}

	using ClientRequest = Network::Request<std::string>;

	void Emplace_Request_Test() {
		std::vector<ClientRequest> requests;

		char buffer[] = "qwerty_12345";

		std::string text{ buffer , 6};
		std::cout << text << std::endl;

		requests.emplace_back(1, buffer, 6);
	}

	//-----------------------------------------------------------------------------------//

	template<typename T, typename... Args>
	auto initFromMoveable(Args&&... args)
	{
		std::vector<std::unique_ptr<T>> vec;
		vec.reserve(sizeof...(Args));
		(vec.emplace_back(std::forward<Args>(args)), ...);
		return vec;
	}


	void CreateVector_UniquePtr() {

		const std::vector<std::unique_ptr<Integer>> m_vec = initFromMoveable<Integer>(
			std::make_unique<Integer>(1),
			std::make_unique<Integer>(2),
			std::make_unique<Integer>(3)
		);
	}

	//-----------------------------------------------------------------------------------//

	void CreateVector_UniquePtr_Lambda() {

        [[maybe_unused]]
		auto init = []<typename T, typename... Args>(Args&&... args) -> auto {
			std::vector<std::unique_ptr<T>> vec;
			vec.reserve(sizeof...(Args));
			(vec.emplace_back(std::forward<Args>(args)), ...);
			return vec;
		};

		/* TODO: Compile error

		const std::vector<std::unique_ptr<Integer>> m_vec = init(
			std::make_unique<Integer>(1),
			std::make_unique<Integer>(2),
			std::make_unique<Integer>(3)
		);
		*/
	}
}


namespace Vector::Allocators {

	template<typename Ty>
	struct MyAllocator {
	public:
		using value_type = Ty;
		using pointer = Ty*;

		static inline constexpr std::align_val_t alignment =
			static_cast<std::align_val_t>(alignof(value_type));

	public:
		[[nodiscart]]
		constexpr pointer allocate(size_t size) {
			std::cout << "allocate(size=" << size << ")\n";
			return static_cast<pointer>(::operator new(size, alignment));
		}

		constexpr void deallocate(pointer ptr, size_t size) {
			std::cout << "deallocate(size=" << size << ")\n";
			::operator delete(static_cast<void*>(ptr), size, alignment);
		}

		template<typename U, typename... Args>
		static void construct(U* ptr, Args&& ... params) {
			std::cout << "construct()\n";
			::new (static_cast<void*>(ptr))U{ std::forward<Args>(params)...};
		}
	};

	//-----------------------------------------------------------//

	struct Long {
        Long(long v) : value{ v } {
        }

		long value;
	};


	struct LongEx {
		long value;

		LongEx(long v) : value{ v } {
		}

		inline long getValue() const noexcept {
			return value;
		}
	};

	//-----------------------------------------------------------//


	void Emplace_CustomType_1() {
		std::vector<Long> values;
		values.emplace_back(123);

		std::cout << values.back().value << std::endl;
	}


	void Emplace_CustomType_2() {
		std::vector<LongEx> values;
		values.emplace_back(123);

		std::cout << values.back().getValue() << std::endl;
	}


	void Emplace_CustomType_Allocator() {
		std::vector<LongEx, MyAllocator<LongEx>> values;
		values.emplace_back(123);

		std::cout << values.back().getValue() << std::endl;
	}
}

namespace Vector::Iterators
{
	void Invalidation_Test()
	{

		std::vector<int> vec {1, 2, 3};

		int& head = vec.front();

		vec.reserve(123);

		//for (int i = 0; i < 10; ++i)
		//	vec.push_back(1);

		std::cout << head << std::endl;
	}
}


void Vector::TestAll()
{
    // Constructors_Tests();

	// Resize_Capacity_Tests();
	// Resize_Capacity_Tests_2();
	// Clear_Capacity_Tests();
	// Allocate_Memory_Tests();

	// Create_and_Print();

	// Insert();
	// Insert_At_TheEnd();
	// Assign_Test();

	// Erase_Tests();
    // Erase_Tests_2();
	// Erase_Tests_Loop();

    // Erase_IF();

	// Emplace_Back_SimpleTest();
	// Emplace_Back_SimpleTest1();
	// Emplace_Back_Tests();
	// Emplace_Back_GOOD_EXAMPLE();

	// Performance_PushBack_vs_EmplaceBack();
	// Compare_Vectors();
	// Sort_Test1();
    // Sort_Test_Pair();
	// Partial_Sort_Test();
	// PopBack();
		
	// Swap();

	// Back_Test();
	// ShrinkToFit_Test();

	// BoolVector::CreateBoolVector();

	// KeepAddresses_IncreaseCapacity();
    // KeepAddresses_IncreaseCapacity_2();

	// ObjectVector::CreateVectorOfObject_NoDefaultConstructor();
	// ObjectVector::CreateVectorOfObject_NoDefaultConstructor_Hack();

    // CustomObjects::PushCustomOjbectToVector();

	// __TESTS__();

	// Eperiments_And_Tests::Emplace_Request_Test();
	// Eperiments_And_Tests::CreateVector_UniquePtr();
	// Eperiments_And_Tests::CreateVector_UniquePtr_Lambda();


	// Allocators::Emplace_CustomType_1();
	// Allocators::Emplace_CustomType_2();
	// Allocators::Emplace_CustomType_Allocator();


	Iterators::Invalidation_Test();
}