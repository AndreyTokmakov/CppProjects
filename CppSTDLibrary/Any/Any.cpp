//============================================================================
// Name        : Any.cpp
// Created on  : 01.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : std::any class src
//============================================================================

#include <iostream>
#include <string>
#include <cstddef>
#include <any>
#include <map>

#include "Any.h"
#include "../Helpers/Helpers.h"

namespace {
    using Helpers::Integer;
}

namespace Any::TestSupport
{
    class FirstType
    {
    protected:
        std::string value;

    public:
        FirstType(std::string val) : value(val) {
            std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
        }
        FirstType(const FirstType &obj) {
            std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
            this->value = obj.value;
        }

        virtual ~FirstType() {
            std::cout << __FUNCTION__ << std::endl;
        }

        virtual std::string getValue() const {
            return this->value;
        }

        virtual void setValue(std::string val) {
            this->value = val;
        }

        //public: /** Operators reload. **/
        //	friend std::ostream& operator<<(std::ostream& stream, const FirstType& integer);
    };




    /** SecondType **/
    class SecondType {
    protected:
        std::string value;

    public:
        SecondType(std::string val) : value(val) {
            std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
        }
        SecondType(const SecondType &obj) {
            std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
            this->value = obj.value;
        }
        virtual ~SecondType() {
            std::cout << __FUNCTION__ << std::endl;
        }

        virtual std::string getValue() const {
            return this->value;
        }

        virtual void setValue(std::string val) {
            this->value = val;
        }
    };

    class ThirdType {
    protected:
        std::string value;

    public:
        ThirdType(std::string val) : value(val) {
            std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
        }
        ThirdType(const ThirdType &obj) {
            std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
            this->value = obj.value;
        }
        virtual ~ThirdType() {
            std::cout << __FUNCTION__ << std::endl;
        }

        virtual std::string getValue() const {
            return this->value;
        }

        virtual void setValue(std::string val) {
            this->value = val;
        }
    };

    class FirstTypeDerived : public FirstType {
    public:
        FirstTypeDerived(std::string val) : FirstType(val) {
            std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
        }
        FirstTypeDerived(const FirstTypeDerived &obj) : FirstType(obj) {
        }

        ~FirstTypeDerived() {
            std::cout << __FUNCTION__ << std::endl;
        }

        std::string getValue() const {
            return "[Derived] : " + this->value;
        }
    };
}

namespace Any {

    template<typename Type>
    void PrintAny(const std::any& var) {
        try {
            std::cout << std::any_cast<Type>(var);
            std::cout << "         [Type: " << typeid(Type).name() << "]" << std::endl;
        }
        catch (const std::bad_any_cast& exception) {
            std::cout << "Failed to cast std::variable to " << typeid(Type).name() << std::endl;
            std::cout << exception.what() << std::endl;
        }
    }



    template<typename T>
    void any_test(const std::any& variable) {
        std::cout << "Size = " << sizeof(variable) << std::endl;
        std::cout << "Size = " << sizeof(T) << " [desizred]" << std::endl;
        std::cout << "Has value = " << std::boolalpha << variable.has_value() << std::endl;

        try {
            // NOTE:  T& var = std::any_cast<T&>(variable); ----> Will cause COMPILE TILE ERROR
            const T& var = std::any_cast<const T&>(variable);
            std::cout << "Value = " << var << std::endl;
        }
        catch (const std::bad_any_cast& exception) {
            std::cout << "Failed to cast std::variable to " << typeid(T).name() << std::endl;
            std::cout << exception.what() << std::endl;
        }
        catch (...) {
            std::cout << "Faiied to cast variable to type '" << typeid(T).name() << "'" << std::endl;
        }
    };

    //--------------------------------------------------------------------//

    void CreateAnd_Null() {
        std::any var(nullptr);
        if (nullptr == std::any_cast<std::nullptr_t>(var)) {
            std::cout << "NULL\n";
        }
    }

    void CreateAndPrint() {
        std::any var(111);

        PrintAny<int>(var);
        PrintAny<std::string>(var);
        PrintAny<Integer>(var);

        std::cout << "\n------------------------------\n" << std::endl;

        std::any var_str(std::string("Hello!"));

        PrintAny<std::string>(var_str);
        PrintAny<int>(var_str);
        PrintAny<Integer>(var_str);
    }

    void LittleDemo() {
        std::any variable(12);
        PrintAny<int>(variable);

        variable = std::string("Hello!");
        PrintAny<std::string>(variable);

        variable = 16;

        PrintAny<int>(variable);
        PrintAny<std::string>(variable);

        std::cout << "\n>>> reset()  \n" << std::endl;

        /** reset and check if it contains any value. **/
        variable.reset();
        if (!variable.has_value()) {
            std::cout << "a is empty!" << "\n";
        }

        std::cout << "\n------------------------------\n" << std::endl;

        // you can use it in a container:
        std::map<std::string, std::any> m;
        m["integer"] = 10;
        m["std::string"] = std::string("Hello World");
        m["float"] = 1.0f;

        for (auto &[key, val] : m) {
            if (val.type() == typeid(int))
                std::cout << "int: " << std::any_cast<int>(val) << "\n";
            else if (val.type() == typeid(std::string))
                std::cout << "std::string: " << std::any_cast<std::string>(val) << "\n";
            else if (val.type() == typeid(float))
                std::cout << "float: " << std::any_cast<float>(val) << "\n";
        }
    }

    void EmplaceTests_Fast_NoCopyConstructor() {
        std::any anyVar;

        anyVar.emplace<TestSupport::FirstType>("FirstTypeTestValue_1");

        try {
            auto& var = std::any_cast<TestSupport::FirstType&>(anyVar);
            std::cout << "VALUE: " << var.getValue() << std::endl;
        }
        catch (const std::bad_any_cast& exc) {
            std::cout << "anyVar variable can not be cast to FirstType type variable.\n" << exc.what() << std::endl;
        }

        anyVar.emplace<TestSupport::SecondType>("SecondTypeTestValue_1");

        try {
            auto& var = std::any_cast<TestSupport::SecondType&>(anyVar);
            std::cout << "VALUE: " << var.getValue() << std::endl;
        }
        catch (const std::bad_any_cast& exc) {
            std::cout << "anyVar variable can not be cast to SecondType type variable.\n" << exc.what() << std::endl;
        }
    }

    void EmplaceTests_Fast_NoCopyConstructor_Ptr() {
        std::any anyVar;

        anyVar.emplace<TestSupport::FirstType>("FirstTypeTestValue_1");

        try {
            auto* var = std::any_cast<TestSupport::FirstType>(&anyVar);
            std::cout << "VALUE: " << var->getValue() << std::endl;
        }
        catch (const std::bad_any_cast& exc) {
            std::cout << "anyVar variable can not be cast to FirstType type variable.\n" << exc.what() << std::endl;
        }

        anyVar.emplace<TestSupport::SecondType>("SecondTypeTestValue_1");

        try {
            auto* var = std::any_cast<TestSupport::SecondType>(&anyVar);
            std::cout << "VALUE: " << var->getValue() << std::endl;
        }
        catch (const std::bad_any_cast& exc) {
            std::cout << "anyVar variable can not be cast to SecondType type variable.\n" << exc.what() << std::endl;
        }
    }

    void EmplaceTests_BAD() {
        std::cout << "************************** BAD: **********************\n" << std::endl;
        {
            std::any anyVar(TestSupport::FirstType("TEST1"));
            try {
                TestSupport::FirstType var = std::any_cast<TestSupport::FirstType>(anyVar);
                std::cout << "VALUE: " << var.getValue() << std::endl;
            }
            catch (const std::bad_any_cast& exc) {
                std::cout << "anyVar variable can not be cast to FirstType type variable" << std::endl;
                std::cout << exc.what() << std::endl;
            }

            anyVar.emplace<TestSupport::SecondType>(TestSupport::SecondType("TEST2"));
            try {
                TestSupport::SecondType var = std::any_cast<TestSupport::SecondType>(anyVar);
                std::cout << "VALUE: " << var.getValue() << std::endl;
            }
            catch (const std::bad_any_cast& exc) {
                std::cout << "anyVar variable can not be cast to SecondType type variable" << std::endl;
                std::cout << exc.what() << std::endl;
            }
        }
        std::cout << "\n************************** GOOD: **********************\n" << std::endl;
        {
            std::any anyVar;
            anyVar.emplace<TestSupport::FirstType>("FirstType_Variable");
            try {
                TestSupport::FirstType& var = std::any_cast<TestSupport::FirstType&>(anyVar);
                std::cout << "VALUE: " << var.getValue() << std::endl;
            }
            catch (const std::bad_any_cast& exc) {
                std::cout << "anyVar variable can not be cast to FirstType type variable" << std::endl;
                std::cout << exc.what() << std::endl;
            }

            anyVar.emplace<TestSupport::SecondType>("SecondType_Variable");
            try {
                TestSupport::SecondType& var = std::any_cast<TestSupport::SecondType&>(anyVar);
                std::cout << "VALUE: " << var.getValue() << std::endl;
            }
            catch (const std::bad_any_cast& exc) {
                std::cout << "anyVar variable can not be cast to SecondType type variable" << std::endl;
                std::cout << exc.what() << std::endl;
            }
        }
    }

    void Swap()
    {
        std::any integer1 = std::make_any<Integer>(111);
        std::any integer2 = std::make_any<Integer>(222);

        std::cout << "integer1 = " << std::any_cast<Integer&>(integer1) << std::endl;
        std::cout << "integer2 = " << std::any_cast<Integer&>(integer2) << std::endl;

        integer2.swap(integer1);

        std::cout << "integer1 = " << std::any_cast<Integer&>(integer1) << std::endl;
        std::cout << "integer2 = " << std::any_cast<Integer&>(integer2) << std::endl;
    }

    void Reset()
    {
        std::any integer1 = std::make_any<Integer>(111);
        integer1.reset();

        std::cout << "EXIT" << std::endl;
    }

    //--------------------------------------------------------------------------------------------------//

    void AnyCast_WithTemplate()
    {
        std::cout << "\n=========================== Test1 ===============================\n";
        {
            auto anyVar = std::make_any<std::string>("Some_Text");
            any_test<std::string>(anyVar);
        }

        std::cout << "\n=========================== Test2 ===============================\n";
        {
            auto anyVar = std::make_any<Integer>(123);
            any_test<Integer>(anyVar);
        }
    }


    //--------------------------------------------------------------------------------------------------//

    void Any_Size_Tests()
    {
        struct SmallObject {
            char buffer[32]{};
        };

        struct BigObject {
            char buffer[1024]{};
        };

        std::cout << "=========================== Test (short) ===============================\n";
        {
            std::any anyVar = std::make_any<short>(1);
            std::cout << "Size = " << sizeof(anyVar) << std::endl;
        }

        std::cout << "\n=========================== Test (double) ===============================\n";
        {
            std::any anyVar = std::make_any<double>(123);
            std::cout << "Size = " << sizeof(anyVar) << std::endl;
        }

        std::cout << "\n=========================== Test (SmallObject) ===============================\n";
        {
            std::any anyVar = std::make_any<SmallObject>();
            std::cout << "Size = " << sizeof(anyVar) << std::endl;
        }

        std::cout << "\n=========================== Test (BigObject) ===============================\n";
        {
            std::any anyVar = std::make_any<BigObject>();
            std::cout << "Size = " << sizeof(anyVar) << std::endl;
        }
    }

    void Has_Value() {
        std::boolalpha(std::cout);

        std::any a0;
        std::cout << "a0.has_value(): " << a0.has_value() << "\n\n";

        std::any a1 = 42;
        std::cout << "a1.has_value(): " << a1.has_value() << '\n';
        std::cout << "a1 = " << std::any_cast<int>(a1) << '\n';
        a1.reset();
        std::cout << "a1.has_value(): " << a1.has_value() << '\n';
    }

    template<typename T>
    void print(const std::any & variable) {
        std::cout << "Size = " << sizeof(variable) << std::endl;
        std::cout << "Size = " << sizeof(T) << " [desizred]" << std::endl;
        std::cout << "Has value = " << std::boolalpha << variable.has_value() << std::endl;
        try {
            const T& var = std::any_cast<const T&>(variable);
            std::cout << "Value = "<< var << std::endl;
        }
        catch (...) {
            std::cout << "Faiied to cast variable to type '" << typeid(T).name() << "'" << std::endl;
        }
    };

    struct Object {
        char buffer[128];
    };

    void __TEST__() {

        /*
        std::cout << "\n=========================== Test0 ===============================\n";
        {

            std::any anyVar = std::make_any<Object>();
            print<Object>(anyVar);
        }
        */

        std::cout << "\n=========================== Test1 ===============================\n";
        {
            auto anyVar = std::make_any<std::string>("Some_Text");
            print<std::string>(anyVar);
        }

        std::cout << "\n=========================== Test2 ===============================\n";
        {
            auto anyVar = std::make_any<Integer>(123);
            print<Integer>(anyVar);
        }



    }
}

namespace Any::Wrapper {

    /*
    template<typename _Type>
    class AnyWrapper {
    private:
        using Getter = std::function<ICalculator& (std::any&)>;

        std::any storage;
        Getter getter;

    public:
        AnyWrapper(std::any) {
            value = std::make_any<std::std::string>("Value1");
        }


    };
    */


    void Test() {
    }
}

void Any::TestAll() {
    // CreateAnd_Null();

     CreateAndPrint();
    // LittleDemo();

    // EmplaceTests_Fast_NoCopyConstructor();
    // EmplaceTests_Fast_NoCopyConstructor_Ptr();

    // EmplaceTests_BAD();

    // Swap();

    // Reset();

    // Has_Value();

    // Any_Size_Tests();

    // AnyCast_WithTemplate();

    // __TEST__();

    // Wrapper::Test();
}