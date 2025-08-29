
#include <iostream>
#include <string>
#include <cassert>
#include <variant>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Variant.h"
#include "../Helpers/Helpers.h"

namespace Variant {

	struct SampleVisitor {
		void operator()(int i) const {
			std::cout << "int: " << i << "\n";
		}
		void operator()(float f) const {
			std::cout << "float: " << f << "\n";
		}
		void operator()(const std::string& s) const {
			std::cout << "string: " << s << "\n";
		}
	};

	class TestUser_String {
	protected:
		std::string name;

	public:
		TestUser_String(const std::string& name) {
			this->name = name;
			std::cout << "Test user created : " << this->name << std::endl;
		}

		const std::string getName() const {
			return name;
		}

		void setName(const std::string& name) {
			this->name = name;
		}

	public:
		friend std::ostream& operator<< (std::ostream& stream, const TestUser_String& user);
	};

	std::ostream& operator << (std::ostream& os, const TestUser_String& user) {
		os << user.name;
		return os;
	}

	class FirstType {
	protected:
		std::string value;

	public:
		FirstType(const std::string& v) : value(v) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		FirstType(const FirstType& obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}

		virtual  ~FirstType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual std::string getValue() const {
			return this->value;
		}

		virtual void setValue(const std::string& v) {
			value = v;
		}
	};

	class SecondType {
	protected:
		std::string value;

	public:
		SecondType(const std::string& v) : value(v) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		SecondType(const SecondType& obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}
		virtual ~SecondType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual std::string getValue() const {
			return this->value;
		}

		virtual void setValue(const std::string& v) {
			value = v;
		}
	};

	class ThirdType {
	protected:
		std::string value;

	public:
		ThirdType(const std::string& v) : value(v) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}
		ThirdType(const ThirdType& obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}
		virtual ~ThirdType() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual std::string getValue() const {
			return this->value;
		}

		virtual void setValue(const std::string& v) {
			value = v;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void Get_Value() {
		using IntStrVar = std::variant<int, std::string>;
		std::vector<IntStrVar> vect {1, "Two", 3, "Four", 5, "Six", 7};

		auto print_variant_var = [&](const auto& variable) {
			if (auto value = std::get_if<std::string>(&variable))
				std::cout << "String  value: " << *value << std::endl;
			else if (auto value = std::get_if<int>(&variable))
				std::cout << "Integer value: " << *value << std::endl;
			else {
				std::cout << "Variable type not String or Integer" << std::endl;
			}
		};

		std::for_each(vect.begin(), vect.end(), print_variant_var);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void VariantCreate_Tests()
	{
		std::variant<int, float> v, w;
		v = 12; // v contains int
		[[maybe_unused]]
		int i = std::get<int>(v);
		w = std::get<int>(v);
		w = std::get<0>(v); // same effect as the previous line
		w = v; // same effect as the previous line

	//  std::get<double>(v); // error: no double in [int, float]
	//  std::get<3>(v);      // error: valid index values are 0 and 1

		try { // w contains int, not float: will throw
			[[maybe_unused]]
			float fValue = std::get<float>(w);
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << exc.what() << std::endl;
		}

		using namespace std::literals;

		std::variant<std::string> variantString("abc");
		// converting constructors work when unambiguous
		variantString = "def"; // converting assignment also works when unambiguous

		std::cout << "variantString value = " << std::get<std::string>(variantString) << std::endl;

		std::variant<std::string, void const*> y("abc");
		// casts to void const * when passed a char const *
		assert(std::holds_alternative<void const*>(y)); // succeeds
		y = "xyz"s;
		assert(std::holds_alternative<std::string>(y)); // succeeds
	}

	void Variant_Vector() {
		using ItsOk = std::variant<std::vector<int>, double>;

		ItsOk variable = std::vector<int>{ 22, 44, 66 }; //set the variant to vector, this constructs the internal vector

		try {
			std::vector<int> v = std::get<std::vector<int>>(variable);
			std::for_each(v.begin(), v.end(), [](int val) { std::cout << val << std::endl; });
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << exc.what() << std::endl;
		}

		variable = 13.7; // reset to double - the internal vector is properly destroyed

		try {
			[[maybe_unused]]
			int i = std::get<std::vector<int>>(variable)[2]; // There's no vector in the variant - throws an exception
		}
		catch (const std::exception& exc) {
			std::cout << "\n" << exc.what();
			if (true == std::holds_alternative<double>(variable)) {
				double d = std::get<double>(variable);
				std::cout << "\nVariable holds Double value: " << d << std::endl;
			}
		}
	}


	void Vector_of_Variant() {
		using VarTYpe = std::variant<std::string, int>;
		std::vector<VarTYpe> varVector {"One", 2, "Three", 4, "Five", 6, "Seven"};

		auto print = [](const auto& var) noexcept-> void {
			if (true == std::holds_alternative<std::string>(var)) 
				std::cout << std::get<std::string>(var) << std::endl;
			else 
				std::cout << std::get<int>(var) << std::endl;
		};

		std::for_each(varVector.cbegin(), varVector.cend(), print);
	}

	void VariantInit3() {
		using varType = std::variant<FirstType, SecondType, ThirdType>;

		varType variable = FirstType("Value1");
		std::cout << " --------------  " << std::endl;

		variable = SecondType("Value2");
		std::cout << " --------------  " << std::endl;

		variable = ThirdType("Value3");
		std::cout << " --------------  " << std::endl;
	}

	void Variant_InitAndGetValue_Tests_1() {
		std::variant<std::string, TestUser_String, long> varStrUserLong;

		try {
			auto value = std::get<std::string>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the STRING type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<long>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the LONG type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<TestUser_String>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the TestUser_String type" << std::endl;
			std::cout << exc.what() << std::endl;
		}

		std::cout << "\n\nvarStrUserLong is set to 102323\n" << std::endl;
		varStrUserLong = 102323;

		try {
			auto value = std::get<std::string>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the STRING type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<long>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the LONG type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<TestUser_String>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the TestUser_String type" << std::endl;
			std::cout << exc.what() << std::endl;
		}

		std::cout << "\n\nvarStrUserLong is set to TestUser_String(TEST_NAME)\n" << std::endl;
		varStrUserLong = TestUser_String("TEST_NAME");

		try {
			auto value = std::get<std::string>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the STRING type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<long>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the LONG type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
		try {
			auto value = std::get<TestUser_String>(varStrUserLong);
			std::cout << "varStrUserLong = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the TestUser_String type" << std::endl;
			std::cout << exc.what() << std::endl;
		}
	}

	void Variant_InitAndGetValue_Tests_2() {
		std::variant<int, std::string> v = "abc";
		std::cout << std::boolalpha << "variant holds int? " << std::holds_alternative<int>(v) << '\n'
			<< "variant holds string? " << std::holds_alternative<std::string>(v) << '\n';
	}

	void Test() {
		std::variant<std::string, int, bool> mySetting = std::string("Hello!");
		mySetting = 42;
		mySetting = false;
	}

	void ChangingValues() {
		std::variant<int, float, std::string> intFloatString{ "Hello" };

		try {
			auto value = std::get<std::string>(intFloatString);
			std::cout << "intFloatString (\"Hello\") = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the STRING type" << std::endl;
			std::cout << exc.what() << std::endl;
		}

		intFloatString = 10; // we're now an int

		try {
			auto value = std::get<int>(intFloatString);
			std::cout << "intFloatString (10) = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the INT type" << std::endl;
			std::cout << exc.what() << std::endl;
		}

		intFloatString.emplace<2>(std::string("Hello")); // we're now string again

		try {
			auto value = std::get<std::string>(intFloatString);
			std::cout << "intFloatString (\"Hello\") = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "varStrUserLong variable holds not the STRING type" << std::endl;
			std::cout << exc.what() << std::endl;
		}

		// std::get returns a reference, so you can change the value:
		std::get<std::string>(intFloatString) += std::string(" World");

		intFloatString = 10.1f;
		if (auto pFloat = std::get_if<float>(&intFloatString); pFloat)
			*pFloat *= 2.0f;
	}

	void ObjectLifetimeTest() {
		class MyType {
		public:
			MyType() { std::cout << "MyType::MyType\n"; }
			~MyType() { std::cout << "MyType::~MyType\n"; }
		};

		class OtherType {
		public:
			OtherType() { std::cout << "OtherType::OtherType\n"; }
			OtherType(const OtherType&) {
				std::cout << "OtherType::OtherType(const OtherType&)\n";
			}
			~OtherType() { std::cout << "OtherType::~OtherType\n"; }
		};


		std::variant<MyType, OtherType> var;
		var = OtherType();
	}

	void Creation_Monostate_Test()
	{
		// default initialization: (type has to has a default ctor)
		std::variant<int, float> intFloat;
		std::cout << intFloat.index() << ", value " << std::get<int>(intFloat) << "\n";

		// monostate for default initialization:

		class NotSimple {
		public:
			NotSimple(int, float) { }
		};

		// std::variant<NotSimple, int> cannotInit; // error
		std::variant<std::monostate, NotSimple, int> okInit;
		std::cout << "Variant (std::monostate). index = " << okInit.index() << std::endl;

		// pass a value:
		std::variant<int, float, std::string> intFloatString{ 10.5f };
		std::cout << "Variant (10.5f). index = " << intFloatString.index() << ", value " << std::get<float>(intFloatString) << std::endl;

		// ambiguity
		// double might convert to float or int, so the compiler cannot decide

		//std::variant<int, float, std::string> intFloatString { 10.5 };

		// ambiguity resolved by in_place
		std::variant<long, float, std::string> longFloatString{ std::in_place_index<1>, 7.6 }; // double!
		std::cout << "Variant (7.6). index = " << longFloatString.index() << ", value " << std::get<float>(longFloatString) << std::endl;

		// in_place for complex types
		std::variant<std::vector<int>, std::string> vecStr{ std::in_place_index<0>, { 0, 1, 2, 3 } };
		std::cout << "Variant ({ 0, 1, 2, 3 } ). index = " << vecStr.index() << ", vector size " << std::get<std::vector<int>>(vecStr).size() << std::endl;

		// copy-initialize from other variant:
		std::variant<int, float> intFloatSecond{ intFloat };
		std::cout << "Variant (intFloat). index = " << intFloatSecond.index() << ", value " << std::get<int>(intFloatSecond) << std::endl;
	}

	struct S {
		S(int i) : i(i) {}
		int i;
	};

	void Monostate_Variant_Tests() {

		// Without the monostate type this declaration will fail.
		// This is because S is not default-constructible.
		std::variant<std::monostate, S> var;

		// var.index() is now 0 - the first element
		// std::get<S> will throw! We need to assign a value
		var = 12;

		std::cout << std::get<S>(var).i << '\n';
	}

	void Variant_Test_Visit()
	{
		std::variant<int, float, std::string> intFloatString;
		static_assert(std::variant_size_v<decltype(intFloatString)> == 3);

		// default initialized to the first alternative, should be 0
		std::visit(SampleVisitor{}, intFloatString);

		// index will show the currently used 'type'
		std::cout << "index = " << intFloatString.index() << std::endl;
		intFloatString = 100.0f;
		std::cout << "index = " << intFloatString.index() << std::endl;
		intFloatString = "hello super world";
		std::cout << "index = " << intFloatString.index() << std::endl;

		// try with get_if:
		if (const auto intPtr(std::get_if<int>(&intFloatString)); intPtr)
			std::cout << "int!" << *intPtr << "\n";
		else if (const auto floatPtr(std::get_if<float>(&intFloatString)); floatPtr)
			std::cout << "float!" << *floatPtr << "\n";

		if (std::holds_alternative<int>(intFloatString))
			std::cout << "the variant holds an int!\n";
		else if (std::holds_alternative<float>(intFloatString))
			std::cout << "the variant holds a float\n";
		else if (std::holds_alternative<std::string>(intFloatString))
			std::cout << "the variant holds a string\n";

		// try/catch and bad_variant_access
		try {
			auto f = std::get<float>(intFloatString);
			std::cout << "float! " << f << "\n";
		}
		catch (std::bad_variant_access&) {
			std::cout << "our variant doesn't hold float at this moment...\n";
		}

		// visit:
		std::visit(SampleVisitor{}, intFloatString);
		intFloatString = 10;
		std::visit(SampleVisitor{}, intFloatString);
		intFloatString = 10.0f;
		std::visit(SampleVisitor{}, intFloatString);
	}

	void Variant_Test_Visit_1()
	{
		// a generic lambda:
		auto PrintVisitor = [](const auto& t) { std::cout << t << std::endl; };

		std::variant<int, float, std::string> intFloatString{ "Hello" };
		std::visit(PrintVisitor, intFloatString);


		intFloatString = 10.5f;
		std::visit(PrintVisitor, intFloatString);

		intFloatString = 100500;
		std::visit(PrintVisitor, intFloatString);
	}

	void Variant_Test_Visit_2()
	{
		class  MultiplyVisitor {
		public:
			float mFactor;

			MultiplyVisitor(float factor) : mFactor(factor) { }

			void operator()(int& i) const {
				i *= static_cast<int>(mFactor);
			}

			void operator()(float& f) const {
				f *= mFactor;
			}

			void operator()(std::string&) const {
				// nothing to do here...
			}
		};

		auto PrintVisitor = [](const auto& t) { std::cout << t << std::endl; };

		std::variant<int, float> intFloat{ 44 };

		//std::visit(MultiplyVisitor(1.5f), intFloat);
		std::visit(PrintVisitor, intFloat);
	}

	class ThrowingClass {
	public:
		explicit ThrowingClass(int i) {
			if (5 == i)
				throw int(10);
		}
		operator int() {
			throw int(10);
		}
	};

	void ExceptionSafetyTest() {
		std::variant<int, ThrowingClass> variable;

		try {
			auto value = std::get<int>(variable);
			std::cout << "'variable' current value = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "'variable' holds not the Integer type." << std::endl;
			std::cout << exc.what() << std::endl;
		}

		std::cout << "******************************" << std::endl;

		try {
			variable = ThrowingClass(5);
		}
		catch (...) {
			std::cout << "catch(...)" << std::endl;
			std::cout << variable.valueless_by_exception() << std::endl;
			std::cout << std::get<int>(variable) << std::endl;
		}

		std::cout << "******************************" << std::endl;

		try {
			auto value = std::get<int>(variable);
			std::cout << "'variable' current value = " << value << std::endl;
		}
		catch (const std::bad_variant_access& exc) {
			std::cout << "'variable' holds not the Integer type." << std::endl;
			std::cout << exc.what() << std::endl;
		}

		std::cout << "******************************" << std::endl;

		// inside emplace
		try {
			variable.emplace<0>(ThrowingClass(10)); // calls the operator int
		}
		catch (...) {
			std::cout << "catch(...)" << std::endl;
			std::cout << variable.valueless_by_exception() << std::endl;
		}
	}

	enum class ErrorCode {
		Ok,
		SystemError,
		IoError,
		NetworkError
	};

	std::variant<std::string, ErrorCode> FetchNameFromNetwork(int i) {
		if (i == 0)
			return ErrorCode::SystemError;
		else if (i == 1)
			return ErrorCode::NetworkError;
		else
			return std::string("Hello World!");
	}

	void ErrorHandlingTest()
	{
		auto response = FetchNameFromNetwork(0);
		if (std::holds_alternative<std::string>(response))
			std::cout << std::get<std::string>(response) << std::endl;
		else
			std::cout << "Error!" << std::endl;

		response = FetchNameFromNetwork(1);
		if (std::holds_alternative<std::string>(response))
			std::cout << std::get<std::string>(response) << std::endl;
		else
			std::cout << "Error!" << std::endl;

		response = FetchNameFromNetwork(10);
		if (std::holds_alternative<std::string>(response))
			std::cout << std::get<std::string>(response) << std::endl;
		else
			std::cout << "Error!" << std::endl;
	}


	void Variant_Size() {
		{
			std::variant<bool, short> variable;
			std::cout << typeid(variable).name() << " | size = " << std::variant_size_v<decltype(variable)> << std::endl;
		}
		{
			std::variant<bool, short, int> variable;
			std::cout << typeid(variable).name() << " | size = " << std::variant_size_v<decltype(variable)> << std::endl;
		}
		{
			std::variant<bool, short, int, long> variable;
			std::cout << typeid(variable).name() << " | size = " << std::variant_size_v<decltype(variable)> << std::endl;
		}
	}

	void Variant_Alternative() {
		std::variant<int, std::string, float, double> variable{"ext"};
		auto types_count = std::variant_size_v<decltype(variable)>;

		std::cout << typeid(std::variant_alternative_t<0, decltype(variable)>).name() << std::endl;
		std::cout << typeid(std::variant_alternative_t<1, decltype(variable)>).name() << std::endl;
		std::cout << typeid(std::variant_alternative_t<2, decltype(variable)>).name() << std::endl;
		std::cout << typeid(std::variant_alternative_t<3, decltype(variable)>).name() << std::endl;
	}

	void Index() {
		std::variant<int, std::string> v = "abc";
		std::cout << "v.index = " << v.index() << '\n';

		v = {};
		std::cout << "v.index = " << v.index() << '\n';
	}

	void Variant_Types_Count() {
		{
			std::variant<int, std::string> var = "abc";
			auto size = std::variant_size_v<decltype(var)>;

			std::cout << "size = " << size << std::endl;
		}

		{
			std::variant<int, float, double> var = 123;
			auto size = std::variant_size_v<decltype(var)>;

			std::cout << "size = " << size << std::endl;
		}
	}
}


namespace Variant::VisitTests
{
	void Simple_Visit_0()
	{
		std::variant<int,double,std::string> v = "hello world!";

		// Generic visit, will instantiate the generic lambda for each type:
		std::visit([](auto&& x) {
			std::cout << x << std::endl;
		}, v);
	}


	void Simple_Visit()
	{
		struct Visitor
		{
			void operator()(int) { std::cout << "int!\n"; }
			void operator()(std::string const&) { std::cout << "string!\n"; }
		};

		Visitor V;

		{
			std::variant<double, bool, std::string> var;
			std::visit(V, var);
		}

		{
			std::variant<double, bool, std::string> var{ std::string{"Test"}};
			std::visit(V, var);
		}
	}

	//------------------------------------------------------------------------

	// Some new types:
	class Triangle {
	public:
		void Render() { std::cout << "Drawing a triangle!\n"; }
	};

	class Polygon {
	public:
		void Render() { std::cout << "Drawing a polygon!\n"; }
	};

	class Sphere {
	public:
		void Render() { std::cout << "Drawing a sphere!\n"; }
	};

	void Polymorphism_Test() {
		std::vector<std::variant<Triangle, Polygon, Sphere>> objects{ Polygon(), Triangle(), Sphere(), Triangle()};

		auto CallRender = [](auto& obj) { obj.Render(); };
		for (auto& obj : objects)
			std::visit(CallRender, obj);
	}

	//--------------------------------------------------------------------------//

	struct Fluid { };
	struct LightItem { };
	struct HeavyItem { };
	struct FragileItem { };

	class VisitPackage {
	public:
		void operator()(Fluid&) { std::cout << "fluid\n"; }
		void operator()(LightItem&) { std::cout << "light item\n"; }
		void operator()(HeavyItem&) { std::cout << "heavy item\n"; }
		void operator()(FragileItem&) { std::cout << "fragile\n"; }
	};

	void VizitTest() {
		std::variant<Fluid, LightItem, HeavyItem, FragileItem> package{ FragileItem() };
		std::visit(VisitPackage(), package);
	}

	//--------------------------------------------------------------------------

	template <class ... FuncType>
	struct overload: FuncType... {
		template <class ... Types>
		overload(Types&& ... params) : FuncType{ std::forward<Types>(params) }... {
			// Impl
		}
		using FuncType::operator()...;
	};

	template <class ...Ts>
	overload(Ts&&...)->overload<std::remove_reference_t<Ts>...>;

	void Visit_With_Overloads() {
		auto int_visit = [](int) { std::cout << "int()\n"; };
		auto float_visit = [](float) { std::cout << "float()\n"; };

		{
			std::variant<int, float, std::string> variant;

			std::visit(overload( int_visit, float_visit,
				[](const std::string& s) { std::cout << "std::string!\n"; } //move only
			), variant);
		}

		{
			std::variant<int, float, std::string> variant{ std::string{"Test"}};

			std::visit(overload(int_visit, float_visit,
				[](const std::string& s) { std::cout << "std::string!\n"; } //move only
			), variant);
		}
	}

    // Helper relying on CTAD. Inherits from constructor arguments and exposes their call operator.
    template <typename ...Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    void Visit_With_Overloads_2()
    {
        std::variant<int,double,std::string> v = "hello world!";

        // Generic visit, will instantiate the generic lambda for each type:
        std::visit([](auto&& x) {
            std::cout << x << "\n";
        }, v);

        v = 2.4;
        // Create a new type by inheriting from lambdas that handle each type:
        std::visit(overloaded{
                [](int& x) {
                    std::cout << "Contains 'int', value: " << x << "\n";
                },
                [](double& x) {
                    std::cout << "Contains 'double', value: " << x << "\n";
                },
                [](std::string& x) {
                    std::cout << "Contains 'std::string', value: " << x << "\n";
                }
        }, v);

    }

    void Vizit_Multiple_Variants()
    {
        std::variant<int, float, char> v1 { 's' };
        std::variant<int, float, char> v2 { 10 };

        std::visit(overload{
                [](int a, int b) {std::cout << "     [](int, int) [" << a << ", " << b << "]\n"; },
                [](int a, float b) { std::cout << "  [](int, float) [" << a << ", " << b << "]\n"; },
                [](int a, char b) { std::cout << "   [](int, char) [" << a << ", " << b << "]\n"; },
                [](float a, int b) { std::cout << "  [](float, int) [" << a << ", " << b << "]\n"; },
                [](float a, float b) { std::cout << "[](float, float) [" << a << ", " << b << "]\n"; },
                [](float a, char b) { std::cout << " [](float, char) [" << a << ", " << b << "]\n"; },
                [](char a, int b) {   std::cout << " [](char, int) [" << a << ", " << b << "]\n"; },
                [](char a, float b) { std::cout << " [](char, float) [" << a << ", " << b << "]\n"; },
                [](char a, char b) {  std::cout << " [](char, char) [" << a << ", " << b << "]\n"; },
        }, v1, v2);
    }
}


namespace Variant::DynamicPolymorphism {

	class Base {
	public:
		virtual ~Base() = default;
		virtual void foo() = 0;
	};

	class X : public Base {
	public:
		virtual void foo() override {
			std::cout << "X::foo()" << std::endl;
		}
	};

	class Y : public Base {
	public:
		virtual void foo() override {
			std::cout << "Y::foo()" << std::endl;
		}

		~Y() override = default;
	};



	class X1 {
	public:
		void foo() {
			std::cout << "X1::foo()" << std::endl;
		}
	};

	class Y1 {
	public:
		void foo() {
			std::cout << "Y1::foo()" << std::endl;
		}
	};


	//--------------------------------------------------------------//

	void Classic_Usage() {
		std::unique_ptr<Base> ptr = std::make_unique<X>();
		ptr->foo();
	}



	void Visit() {

		auto visit_foo = [](auto& obj) {
			std::visit([](auto&& v) {
				v.foo();
			}, obj);
		};

		std::variant<X1, Y1> var;
		visit_foo(var);

		var = Y1();
		visit_foo(var);
	}

	//--------------------------------------------------------------//

	struct Object {
		virtual void print() const noexcept = 0;
	};

	struct Circle: public Object {
		virtual void print() const noexcept override {
			std::cout << "Circle::Draw()\n";
		}
	};

	struct Square : public Object {
		virtual void print() const noexcept override {
			std::cout << "Square::Draw()\n";
		}
	};

	using Type = std::variant<Circle, Square>;

	void PrintElements(const std::vector<Type>& vect) {
		for (const Type& obj : vect) {
			std::visit([](const auto& o) { o.print(); }, obj);
		}
	}

	void Polimorph_Visit()
    {
		std::vector<Type> objects {Circle{}, Square{}, Square{}};
		PrintElements(objects);
	}
}

namespace Variant::DynamicPolymorphism_Demo2
{
    struct Shape {
        virtual double area() const = 0;
        virtual ~Shape() {}
    };

    struct CircleShape: public Shape {
        double radius;
        explicit CircleShape(double radius): radius{radius} {}
        double area() const override { return 3.14 * radius * radius; }
    };

    struct RectangleShape: public Shape {
        double height, width;
        explicit RectangleShape(double height, double width): height{height}, width{width} {}
        double area() const override { return height * width; }
    };

    struct SquareShape: public Shape {
        double side;
        explicit SquareShape(double side): side{side} {}
        double area() const override { return side * side; }
    };

    void classicPointerDispatch()
    {
        std::vector<std::unique_ptr<Shape>> shapes;
        {
            shapes.push_back(std::make_unique<CircleShape>(3.));
            shapes.push_back(std::make_unique<RectangleShape>(4., 5.));
            shapes.push_back(std::make_unique<SquareShape>(6.));
        }

        double sum = 0.0f;
        for (const auto& shape : shapes) {
            sum += shape->area();
        }

        std::cout << "sum: " << sum << std::endl;
    }


    //---------------------------------------------------------------------------------

    struct Circle final {
        double radius;
        double area() const { return 3.14 * radius * radius; }
    };

    struct Rectangle final {
        double height, width;
        double area() const { return height * width; }
    };

    struct Square final {
        double side;
        double area() const { return side * side; }
    };

    void variantVisitDispatch()
    {
        std::vector<std::variant<Circle, Rectangle, Square>> shapes;
        {
            shapes.emplace_back(Circle{3.});
            shapes.emplace_back(Rectangle{4., 5.});
            shapes.emplace_back(Square{6.});
        }

        double sum = 0.0f;
        for (const auto& shape : shapes) {
            std::visit([&sum](const auto& shape) { sum += shape.area(); }, shape);
        }

        std::cout << "sum: " << sum << std::endl;
    }

    void Visit_Vs_Pointer()
    {
        classicPointerDispatch();
        variantVisitDispatch();
    }
}

namespace Variant::DynamicPolymorphism
{
    struct Element {
        std::string name {};
    };

    std::vector<Element> getElements() {
        return {
                Element{"ElementOne"},
                Element{"ElementTwo"},
                Element{"ElementThree"},
                Element{"ElementFour"},
                Element{"ElementFive"}
        };
    }

    struct HandlerA {
        void handle(const Element &element) const {
            std::cout << "HandlerA: " << element.name << std::endl;
        }
    };

    struct HandlerB {
        void handle(const Element &element) const {
            std::cout << "HandlerB: " << element.name << std::endl;
        }
    };

    std::variant<HandlerA, HandlerB> getHandler()
    {
        return HandlerA{};
    }

    void ProcessElementsList()
    {
        const auto handler = getHandler();
        const std::vector<Element> elements = getElements();
        std::visit([&elements](const auto& h){
            for (const Element& el: elements)
                h.handle(el);
        }, handler);
    }
}

namespace Variant::Experiments {

	void Map_Variant_Keys() {
		using Key = std::variant<int, std::string>;
		using Value = std::string;

		std::unordered_map<Key, Value> kvs;

		kvs[Key{ 1 }] = "One";
		kvs[Key{ "1" }] = "One";

		std::cout << kvs[Key{ 1 }] << " " << kvs[Key{ "1" }] << "\n"; //One One
	}
}


namespace Variant::Experiments_DNS_Response {

    struct NS {
        std::string name = "NS";
    };

    struct SOA {
        std::string name = "SOA";
    };

    struct CNAME {
        std::string name = "CNAME";
    };

    struct AAAA {
        std::string name = "AAAA";
    };

    struct Visitor {
        void operator()(const std::string& s) { std::cout << "string:" << s << "\n"; }
        void operator()(const NS& obj) { std::cout << "NS!\n"; }
        void operator()(const SOA& obj) { std::cout << "SOA: " << obj.name << "\n"; }
        void operator()(const CNAME& obj) { std::cout << "CNAME!\n"; }
        void operator()(const AAAA& obj) { std::cout << "AAAA!\n"; }

    };

    void Tests() {
        std::variant<std::string, NS, SOA, CNAME, AAAA> resourceData;


        auto& res = resourceData.emplace<SOA>();
        res.name = "2345";

        // auto& res = std::get<std::string>(resourceData);
        // res.append("sdsdsd");

        std::visit(Visitor{}, resourceData);
    }
}


namespace Variant::Heterogeneous_Message_Handler
{
	/** https://biowpn.github.io/bioweapon/2025/08/20/heterogeneous-message-handler.html **/

	struct MessageA { /** **/ };
	struct MessageB { /** **/ };
	struct MessageC { /** **/ };
	struct MessageD { /** **/ };
	struct MessageE { /** **/ };
	struct MessageEx { /** **/ };

	class  Processor
	{
		template <class T>
		void process(const T&) = delete("Unhandled message");

	public:

		using AnyMessage = std::variant<MessageA, MessageB, MessageC, MessageD, MessageE>;

		void handle(const AnyMessage& any_msg);
	};


	template<>
	void Processor::process(const MessageA&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	template<>
	void Processor::process(const MessageB&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	template<>
	void Processor::process(const MessageC&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	template<>
	void Processor::process(const MessageD&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	template<>
	void Processor::process(const MessageE&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	/// The specializations must be visible before they are used, otherwise they won’t be selected.
	/// Member templates aren’t exception to this rule.

	void Processor::handle(const AnyMessage& any_msg)
	{
		std::visit([this](const auto& msg) {
			process(msg);
		}, any_msg);
	}

	void Test()
	{
		Processor::AnyMessage msg;
		Processor p;
		p.handle(msg);
	}
}


void Variant::TestAll()
{
	// Variant_Tests::VariantCreate_Tests();

	// Variant_Vector();
	// Vector_of_Variant();

	// Get_Value();

	// Variant_Size();

	// Variant_Alternative();

	// Index();

	// Variant_Types_Count();

	// VariantInit3();
	// Creation_Monostate_Test();

	// Monostate_Variant_Tests();

	// ChangingValues();
	 
	// ObjectLifetimeTest();

	// Variant_Test_Visit();
	// Variant_Test_Visit_1();
	// Variant_Test_Visit_2();

	// ExceptionSafetyTest();
	// ErrorHandlingTest();

	Heterogeneous_Message_Handler::Test();

	// VisitTests::Simple_Visit_0();
	// VisitTests::Simple_Visit();
	// VisitTests::Polymorphism_Test();
	// VisitTests::VizitTest();
	// VisitTests::Visit_With_Overloads();
	// VisitTests::Visit_With_Overloads_2();
	// VisitTests::Vizit_Multiple_Variants();


    // Variant_InitAndGetValue_Tests_1();
    // Variant_InitAndGetValue_Tests_2();


	// DynamicPolymorphism::Classic_Usage();
	// DynamicPolymorphism::Visit();
	// DynamicPolymorphism::Polimorph_Visit();
	// DynamicPolymorphism::ProcessElementsList();
	// DynamicPolymorphism_Demo2::Visit_Vs_Pointer();


    // Experiments_DNS_Response::Tests();
	// Experiments::Map_Variant_Keys();
};