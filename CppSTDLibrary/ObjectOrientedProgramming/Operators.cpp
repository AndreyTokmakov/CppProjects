/**============================================================================
Name        : Operators.cpp
Created on  : 07.05.2024
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ OPP Operators
============================================================================**/

#include "Operators.h"

#include <iostream>

namespace OperatorCall_ExplicitTypeSpecialization
{
    struct Data1 {
    };

    template<typename T>
    struct StorageOptions
    {
        void ignore_missing([[maybe_unused]] bool ignore) {
            // Some logic
        }
    };

    struct StorageLoader
    {
        template<typename DataType>
        DataType operator()(const StorageOptions<DataType>*)
        {
            return DataType{};
        }
    };

    void Test()
    {
        StorageLoader storageLoader;

        {
            StorageOptions<Data1> dataOptions;
            dataOptions.ignore_missing(true);

            Data1 data = storageLoader(&dataOptions);
            std::cout << typeid(data).name() << std::endl;
        }

        {
            Data1 data = storageLoader.operator()<Data1>(nullptr);
            std::cout << typeid(data).name() << std::endl;
        }

        {
            Data1 obj;
            Data1 data = storageLoader.operator()<decltype(obj)>(nullptr);
            std::cout << typeid(data).name() << std::endl;
        }
    }
}


namespace OperatorOverloading
{

    class Long {
        long value{ 0 };

    public:
        Long() = default;

        explicit Long(long v) : value{ v } {
            std::cout << "Long::Long(" << value << ")\n";
        }

        ~Long() {
            std::cout << "Long::~Long(" << value << ")\n";
        }

        Long(const Long& l) = delete;
        Long& operator=(const Long& l) = delete;

        Long(Long&& l) noexcept = delete;
        Long& operator=(Long&& l) noexcept = delete;

        friend Long operator+(const Long& l1, const Long& l2);


        friend std::ostream& operator<<(std::ostream& stream, const Long& obj);
    };

    std::ostream& operator<<(std::ostream& stream, const Long& obj) {
        stream << obj.value;
        return stream;
    }

    Long operator+(const Long& l1, const Long& l2) {
        return Long(l1.value + l2.value);
    }

    //------------------------------------------------------------------

    void Tests() {
        Long v1{ 11 };
        std::cout << v1 << std::endl;

        Long v2{ 22 };
        auto x = (v1 + v2);
        std::cout << x << std::endl;

        // Long v3 = std::move(v2);
    }
}

namespace Operator_Overload_Tests
{

    struct Base
    {
        std::string text;

    public:
        virtual ~Base() = default;

    public:
        explicit Base(std::string str) : text(std::move(str)) {
            std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
        }

        Base& operator=(const Base& obj) noexcept {
            std::cout << "Base: Assignment operator called. Value = " << text << std::endl;
            this->text = obj.text;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& stream, const Base& base) {
            stream << base.text;
            return stream;
        }
    };

    /** Base class definition: **/
    class Derived: public Base {
    public:
        std::string description;

    public:
        Derived(const std::string& str, const std::string& desc) : Base(str), description(desc) {
            std::cout << "Derived: Constructor called. { " << text  << ", " << description << " }" << std::endl;
        }

        /*
        Derived& operator=(Derived& obj) noexcept {
            std::cout << "Derived: Assignment operator called. Value = " << text << std::endl;
            this->description = obj.description;
            return *this;
        }*/

        friend std::ostream& operator<<(std::ostream& stream, const Derived& obj) {
            stream << "{ " << obj.text << ", " << obj .description << " }";
            return stream;
        }
    };


    void Inherit_Copy_Assignment_Operator()
    {

        /*  // Constructor src:
        {
            Base obj1("Value1");
            std::cout << obj1 << std::endl;

            Base obj2 = std::string("Value2");
            std::cout << obj2 << std::endl;
        }
        */

        {
            Base obj1("Value1"), obj2("Value2");
            std::cout << std::endl;

            std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
            obj1 = obj2;
            std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
        }

        std::cout << "\n ----------------- Test2 ----------------------" << std::endl << std::endl;
        {
            Derived obj1("Value111", "Desc111"), obj2("Value2222", "Desc2222");

            std::cout << std::endl;

            std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
            obj1 = obj2;
            std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
        }
    }
}




void Operators::TestAll()
{
    // OperatorCall_ExplicitTypeSpecialization::Test();

    // OperatorOverloading::Tests();

    Operator_Overload_Tests::Inherit_Copy_Assignment_Operator();
}
