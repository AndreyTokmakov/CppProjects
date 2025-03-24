/**============================================================================
Name        : main.cpp
Created on  : 30.04.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Software design patterns C++ test project
============================================================================**/

#include <chrono>
#include <thread>

#include "AccessToken/AccessToken.h"
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
#include "PImpl/PImpl.h"
#include "Command/Command.h"
#include "ChainOfResponsibility/ChainOfResponsibility.h"
#include "Decorator/Decorator.h"
#include "Observer/Observer.h"
#include "FabricMethod/FabricMethod.h"
#include "ExecutorAdapter/ExecutorAdapter.h"
#include "Proxy/Proxy.h"
#include "Singleton/Singleton.h"
#include "State/State.h"
#include "Mediator/Mediator.h"
#include "Memento/Memento.h"
#include "Monostate/Monostate.h"
#include "Strategy/Strategy.h"
#include "Visitor/Visitor.h"
#include "Named_Parameter_Idiom/NamedParameter.h"
#include "CRTP/CRTP.h"
#include "TypeErasure/TypeErasure.h"
#include "TagDispatching/TagDispatching.h"
#include "Policy/Policy.h"
#include "RAII/RAII.h"
#include "NVI/NVI_SimpleExample.h"
#include "VirtualConstructor/VirtualConstructor.h"
#include "TemplateMethod/TemplateMethod.h"
#include "LazyInitialization/LazyInitialization.h"
#include "MVC/ModelViewController.h"
#include "DependencyInjection/DependencyInjection.h"
#include "TypeID/TypeID.h"
#include "FactoryClasses/FactoryClasses.h"


int main([[maybe_unused]]  int argc,
         [[maybe_unused]]  char** argv)
{
    /** Creation patterns **/
    // AbstractFactory::TestAll();
    // Builder::TestAll();
    // FabricMethod::TestAll();
    // FactoryClasses::TestAll();
    // Prototype::TestAll();
    // Singleton::TestAll();;
    // Monostate::TestAll();;
    // ObjectPools::TestAll();
    // DependencyInjection::TestAll();
    // LazyInitialization::TestAll();

    /** Structural patterns: **/
    // Adapter::TestAll();
    // Bridge::TestAll();
    // PImpl::TestAll();
    // Composite::Test();
    // Facade::Test();
    // Decorator::Test();
    // Flyweight::Test();
    // ModelViewController::TestAll();
    // Proxy::CarTest();
    // Proxy::DocumentTest();
    // Proxy::SubjectTest();
    // Proxy::DatabaseProxyTest();

    /** Behaviour patterns **/
    // Strategy::Test();
    // Policy::Test();
    // ChainOfResponsibility::TestAll();
    // Command::TestAll();
    // Iterator::Test();
    // Interpreter::Test();
    // Mediator::TestAll();
    // Memento::TestAll();
    // Observer::TestAll();
    // State::TestAll();
    // TemplateMethod::TestAll();
    // Visitor::test();
    /// NULL OBJECT

    /** C++ IDIOMS: **/
    // CRTP::Test();
    // NVI_SimpleExample::TestAll();
    // TypeErasure::Test();
    // RAII::Test();
    // CopyAndSwap::TEST_ALL();   ---- TESTs
    // TagDispatching::Test();
    // VirtualConstructor::Test();
    TypeID::TestAll();


    // AccessToken::TestAll();


    // ExecutorAdapter::TestAll();
	// NamedParameter::Test();
    // Filter::TestAll();
}

