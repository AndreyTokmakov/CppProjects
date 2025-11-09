    /**============================================================================
Name        : Class_Member_Pointer.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Class Member Pointer
============================================================================**/

#include <iostream>
#include <string>
#include "Utilities.hpp"


namespace
{
    struct Payment
    {
        double amount { 0.0 };
        std::string category;

        auto operator<=>(const Payment& other) const = default;
    };


    void getPointerToClassMember()
    {
        double Payment::*ptr = &Payment::amount; // Here!

        Payment payment{3.14, "Groceries"};
        std::cout << payment.*ptr << std::endl;

        // OUTPUT:
        // 3.14
    }

    /**
    double Payment::*       ptr         = &Payment::amount;
    // тип указателя    имя указателя    инициализатор

    По сути это особый тип указателя, который хранит смещение поля относительно начала объекта в байтах.
    Это не специфицировано в стандарте, но примерно везде так работает.

    Мы обязательно должны указать, на какой тип полей этот указатель может указывать.
    Таким образом указатель ptr может указывать на любое поле класса Payment, имеющее тип double. То есть:

    struct Type {
        int a;
        int b;
        float c;
    };

    int Type::*p = nullptr;

    p = &Type::a;  // OK, a is int
    p = &Type::b;  // OK, b is int
    p = &Type::c;  // <--------------------- ERROR! c is float
    **/
}


void Utilities::Class_Member_Pointer::TestAll()
{
    getPointerToClassMember();
}