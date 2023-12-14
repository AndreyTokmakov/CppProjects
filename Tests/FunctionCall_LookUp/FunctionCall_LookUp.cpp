/**============================================================================
Name        : FunctionCall_LookUp.cpp
Created on  : 04.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FunctionCall_LookUp.cpp
============================================================================**/

#include "FunctionCall_LookUp.h"

#include <iostream>
#include <string_view>
#include <source_location>

#define INVOKE_INFO_1  constexpr std::source_location location = std::source_location::current(); \
    std::cout << location.file_name() << ' ' << location.function_name() << ' ' << location.line() << std::endl;

#define INVOKE_INFO_2  std::cout << __FILE_NAME__ << ':' << __LINE__ << " [ " \
        << std::source_location::current().function_name() << " ]\n";

#define INVOKE_INFO INVOKE_INFO_2


// https://preshing.com/20210315/how-cpp-resolves-a-function-call/
namespace FunctionCall_LookUp::Demo1
{

    namespace galaxy
    {
        struct Asteroid {
            float radius = 12;
        };

        void blast([[maybe_unused]] Asteroid *ast,
                   [[maybe_unused]] float force)
        {
            INVOKE_INFO
        }
    }

    struct Target
    {
        galaxy::Asteroid *ast;

        Target(galaxy::Asteroid *ast) : ast{ast} {}

        operator galaxy::Asteroid *() const { return ast; }
    };

    bool blast([[maybe_unused]] Target target)
    {
        INVOKE_INFO
        return true;
    }

    template<typename T>
    void blast(T *obj, float force)
    {
        INVOKE_INFO
    }

    void play(galaxy::Asteroid* ast)
    {    /**
         * The reason is because any time you use an unqualified name in a function call – and the name
         * doesn’t refer to a class member, among other things – ADL kicks in,  and name lookup becomes more greedy.
         * Specifically, in addition to the usual places, the compiler looks for candidate functions
         * in the namespaces of the argument types – hence the name “argument-dependent lookup”.
         **/

        /// argument type is galaxy::Asteroid* --> the galaxy namespace is searched for candidate functions (ADL)

        blast(ast, 100); // argument-dependent lookup | galaxy::blast() --> will be called | because of galaxy::Asteroid
    }


    void test()
    {
        play(new galaxy::Asteroid());
    }
};

namespace FunctionCall_LookUp::Template_vs_NonTemplate
{

    void foo([[maybe_unused]] int a,
             [[maybe_unused]] int b)
    {
        INVOKE_INFO
    }

    template<typename T1, typename T2>
    void foo([[maybe_unused]] T1 a,
             [[maybe_unused]] T2 b)
    {
        INVOKE_INFO
    }

    void test()
    {
        // FunctionCall_LookUp.cpp:86
        // [ void FunctionCall_LookUp::Template_vs_NonTemplate::foo(int, int) ]
        foo(1, 1); /// Non-template function wins

        // FunctionCall_LookUp.cpp:92
        // [ void FunctionCall_LookUp::Template_vs_NonTemplate::foo(T1, T2) [with T1 = double; T2 = double] ]
        foo(1.0, 1.0);
    }
};

void FunctionCall_LookUp::TestAll()
{
    // Demo1::test();
    Template_vs_NonTemplate::test();

};