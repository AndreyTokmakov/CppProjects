//============================================================================
// Name        : mainn.cpp
// Created on  : 30.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Software design patterns C++ test project
//============================================================================

#include <chrono>
#include <thread>

#include "CopyAndSwap/CopyAndSwap.h"
#include "Prototype/Prototype.h"
#include "Interpreter/Interpreter.h"
#include "Iterator/Iterator.h"
#include "Composite/Composite.h"
#include "AbstractFactory/AbstractFactory.h"
#include "Adapter/Adapter.h"
#include "Builder/Builder.h"
#include "ObjectPools/ObjectPools.h"
#include "Filter/Filter.h"
#include "Flyweight/Flyweight.h"
#include "Facade//Facade.h"
#include "Bridge/Bridge.h"
#include "Command/Command.h"
#include "ChainOfResponsibility/ChainOfResponsibility.h"
#include "Decorator/Decorator.h"
#include "Observer/Observer.h"
#include "FabricMethod/FabricMethod.h"
#include "ExecutorAdapter/ExecutorAdapter.h"
#include "Proxy/Proxy_Pattern_Tests.h"
#include "Singleton/Singleton.h"
#include "State/State.h"
#include "Mediator/Mediator.h"
#include "Memento/Memento.h"
#include "Strategy/Strategy.h"
#include "Visitor/Visitor.h"
#include "Visitor/StockVisitor.h"
#include "Named_Parameter_Idiom/NamedParameter.h"
#include "CRTP/CRTP_Tests.h"
#include "TypeErasure/TypeErasure.h"
#include "TagDispatching/TagDispatching.h"
#include "Policy/Policy.h"
#include "RAII/RAII.h"
#include "VirtualConstructor/VirtualConstructor.h"
#include "TemplateMethod/TemplateMethod.h"


int main([[maybe_unused]]  int argc,
         [[maybe_unused]]  char** argv)
{
    /** Creation patterns **/
    // AbstractFactory::Test();
    // Builder::Test();
    // Builder::Test_HTML();
    // Builder::Test_User();
    // FabricMethod::TestAll();
    // Prototype::Test();
    // Singleton::TEST_ALL();;
    // ObjectPools::SimpleTest();
    // ObjectPools::ExtentedTest();

    /** Behaviour patterns **/
    // Strategy::Test();
    // ChainOfResponsibility_Pattern_Tests::Test();
    // Command::TestAll();
    // Iterator::Test();
    // Mediator::TestAll();
    // Memento::TestAll();
    // Observer::TestAll();
    // Observer_Tests_Ex::Test();
    // State::TestAll();
    // TemplateMethod::TestAll();


    /** Structural patterns: **/
    // Adapter::TestAll();
    // Bridge::TestAll();
    Composite::Test();
    // Facade::Test();


    /** C++ IDIOMS: **/
    // CRTP_Tests::Test();
    // TypeErasure::Test();
    // RAII::Test();
    // CopyAndSwap::TEST_ALL();   ---- TESTs
    // TagDispatching::Test();    ---- TESTS
    // VirtualConstructor::Test();




    // ExecutorAdapter::TestAll();



    // Policy::Test();

	// Interpreter::Test();


    // Flyweight::TestDriver();


	// DecoratorTests::Test();

	// Proxy_Pattern_Tests::Test();
	// Proxy_Pattern_Tests::Test2();
	// Proxy_Pattern_Tests::Test3();


	// Visitor_Pattern_Tests::test();
	// StockVisitor::test();


	// NamedParameter::Test();


    // Filter::Test();
}

