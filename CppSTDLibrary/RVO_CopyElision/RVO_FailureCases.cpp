/**============================================================================
Name        : RVO_FailureCases.cpp
Created on  : 09.08.2024
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : RVO_FailureCases
============================================================================**/

#include "RVO_FailureCases.h"

#include <iostream>
#include <array>
#include <numeric>

#include "../Helpers/Helpers.h"

using Helpers::Integer;
using Helpers::Long;

namespace Common
{
    struct Data
    {
        int value { 0 };

        Data() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        explicit Data(const int v): value {v} {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        ~Data() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data(const Data&) {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data(Data&&) noexcept {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data& operator=(const Data&) {
            std::cout << __PRETTY_FUNCTION__ << std::endl; return *this;
        }

        Data& operator=(Data&&) noexcept {
            std::cout << __PRETTY_FUNCTION__ << std::endl; return *this;
        }

    };
}

namespace RVO_FailureCases::If_Else_Branch
{
    using namespace Common;

    Data getData(const int value, const bool option)
    {
        if (option)
        {
            Data data;
            data.value = value;
            return data;
        }
        else {
            Data data;
            data.value = value * 10;
            return data;
        }
    }

    void Test()
    {
        Data data = getData(10, true);
    }
}

namespace RVO_FailureCases::BAD
{
    Integer get_integer_1(int v)
    {
        return Integer {v};
    }

    Integer get_integer_2(Integer&& obj)
    {
        return obj;
    }

    template<typename T>
    Integer get_integer_3(T&& obj)
    {
        return std::forward<T>(obj);
    }

    void When_Return_Passed_Value()
    {
        std::cout <<   "----------------- OK ----------------\n"; {
            Integer obj = get_integer_1(1);
        }

        std::cout <<   "----------------- BAD  ----------------\n"; {
            Integer obj = get_integer_2({2});
        }

        std::cout <<   "----------------- BAD  ----------------\n"; {
            Integer obj = get_integer_3(Integer{2});
        }
    }
}

namespace RVO_FailureCases::BAD
{
    Long moving_out_local_variable()
    {
        Long obj;
        return std::move(obj);
    }

    void Move_LocalVariable()
    {
        Long obj = moving_out_local_variable();
        std::cout << obj.getValue() << std::endl;
    }
}

namespace RVO_FailureCases::BAD
{
    template<class T>
    T _get_passed_object(T obj)
    {
        return obj;
    }

    void Return_Passed_Object()
    {
        Long obj = _get_passed_object(Long(123));
    }

    void Return_Passed_Object_Back()
    {
        Long l(123);
        Long obj = _get_passed_object(l);
    }
}

namespace RVO_FailureCases::BAD
{
    Integer _getInteger_FromVector(size_t index)
    {
        std::vector<Integer> nums;
        nums.reserve(10);
        for (int i : {1, 2, 3, 4, 5})
            nums.emplace_back(i);

        return std::move(nums[index]);
    }

    void GetObject_FromVector()
    {
        Integer&& integer = _getInteger_FromVector(2);
        std::cout << integer.getValue() << std::endl;
    }
}


namespace RVO_FailureCases::WrongType_Inheritance
{
    struct LongEx : public Long
    {
        explicit LongEx(int val = 0) : Long { val } {
            std::cout << "LongEx(" << this->value << ")" << std::endl;
        }

        virtual ~LongEx() {
            std::cout << "~LongEx(" << this->value << ")" << std::endl;
        }

        LongEx(const LongEx& obj)  : Wrapper(obj)
        {
            std::cout << "LongEx(" << this->value << ") noexcept: move constructor" << std::endl;
        }

        LongEx(LongEx&& obj) noexcept: Wrapper(std::move(obj))
        {
            std::cout << "LongEx(" << this->value << ") noexcept: move constructor" << std::endl;
        }

        LongEx& operator=(const LongEx& right)
        {
            value = right.value;
            std::cout << "LongEx(" << this->value << ") noexcept: move constructor" << std::endl;
            return *this;
        }

        LongEx& operator=(LongEx&& right) noexcept
        {
            value = std::exchange(right.value, 0);
            std::cout << "LongEx(" << this->value << ") noexcept: move constructor" << std::endl;
            return *this;
        }
    };

    Long get_wrong_type_object_RVO()
    {
        return LongEx {1};
    }

    Long get_wrong_type_object_RNVO()
    {
        LongEx obj(2);
        return obj;
    }

    LongEx get_object_RVO()
    {
        return LongEx {3};
    }

    LongEx get_object_RNVO()
    {
        LongEx obj(4);
        return obj;
    }

    void Failure_Inheritance()
    {
        std::cout <<   "----------------- BAD: RVO Return wrong type ----------------\n"; {
            Long obj = get_wrong_type_object_RVO();
        }

        std::cout <<   "----------------- BAD: RNVO Return wrong type ----------------\n"; {
            Long obj = get_wrong_type_object_RNVO();
        }

        std::cout <<   "----------------- OK : RVO Return wrong type ----------------\n"; {
            LongEx obj = get_object_RVO();
        }

        std::cout <<   "----------------- OK : RNVO Return wrong type ----------------\n"; {
            LongEx obj = get_object_RNVO();
        }
    }
}


namespace RVO_FailureCases::Multiple_Objects
{
    Integer get_integer_multiple_objs_if_else_BAD(int v)
    {
        Integer one = Integer {1};
        Integer other = Integer {v};

        if (1 == v)
            return one;

        return other;
    }

    Integer get_integer_multiple_objs_if_else_OK(int v)
    {
        Integer obj = Integer {v};
        if (1 == v)
            obj.value = v;
        return obj;
    }

    Integer get_integer_multiple_objs_if_else_OK_Multiple_Return(int v)
    {
        Integer obj = Integer {0};
        if (1 == v)
        {
            obj.value = 1;
            return obj;
        }
        else
        {
            obj.value = v;
            return obj;
        }
    }

    void Return_One_Of_Multiple_objects()
    {
        std::cout <<   "----------------- BAD ----------------\n"; {
            Integer obj = get_integer_multiple_objs_if_else_BAD(1);
        }
        std::cout <<   "----------------- OK ----------------\n"; {
            Integer obj = get_integer_multiple_objs_if_else_OK(1);
        }
        std::cout <<   "----------------- OK ----------------\n"; {
            Integer obj = get_integer_multiple_objs_if_else_OK_Multiple_Return(1);
        }
    }
}

// TODO:  [https://youtu.be/WyxUilrR6fU?t=1136]
void RVO_FailureCases::TestAll()
{
    // If_Else_Branch::Test();

    // WrongType_Inheritance::Failure_Inheritance();

    Multiple_Objects::Return_One_Of_Multiple_objects();

    // BAD::When_Return_Passed_Value();

    // BAD::Move_LocalVariable();

    // BAD::Return_Passed_Object();
    // BAD::Return_Passed_Object_Back();

    // BAD::GetObject_FromVector();


}
