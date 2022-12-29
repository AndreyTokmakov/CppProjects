cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DesignPatterns)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(DesignPatterns
    main.cpp
    AbstractFactory/AbstractFactory.cpp AbstractFactory/AbstractFactory.h
    Adapter/Adapter.cpp Adapter/Adapter.h Adapter/USBAdapter.cpp
    Bridge/Bridge.cpp Bridge/Bridge.h
    Builder/Builder.cpp Builder/Builder.h
    ChainOfResponsibility/ChainOfResponsibility.cpp ChainOfResponsibility/ChainOfResponsibility.h
    Command/Command.cpp Command/Command.h
    Composite/Composite.cpp Composite/Composite.h
    CRTP/CRTP_Tests.cpp CRTP/CRTP_Tests.h
    Decorator/Decorator.cpp Decorator/Decorator.h
    FabricMethod/FabricMethod.h FabricMethod/FabricMethod.cpp  FabricMethod/FabricMethod2.cpp
	Facade/Facade.cpp Facade/Facade.h
    Filter/Filter.cpp Filter/Filter.h
    Flyweight/Flyweight.cpp Flyweight/Flyweight.h Flyweight/Flyweight_Icon.cpp Flyweight/Flyweight_Icon.h
    Interpreter/Interpreter.cpp Interpreter/Interpreter.h
    Mediator/Mediator.cpp Mediator/Mediator.h
    Memento/Memento.cpp Memento/Memento.h
    Named_Parameter_Idiom/NamedParameter.cpp Named_Parameter_Idiom/NamedParameter.h
	ObjectPools/ObjectPoolsSimple.cpp  ObjectPools/ObjectPoolsExtended.cpp ObjectPools/ObjectPools.h
    Observer/Observer.cpp Observer/Observer.h
    Prototype/Prototype.cpp Prototype/Prototype.h
	Proxy/Document.cpp Proxy/Document.h
	Proxy/IPage.h 
	Proxy/Page.cpp Proxy/Page.h 
	Proxy/PageProxy.cpp Proxy/PageProxy.h 
	Proxy/Proxy.cpp Proxy/Proxy.h 
	Proxy/Proxy_Pattern_Tests.cpp Proxy/Proxy_Pattern_Tests.h 
	Proxy/Subject.cpp Proxy/Subject.h 
    RAII/RAII.cpp
    Singleton/Singleton.cpp Singleton/Singleton.h
	ExecutorAdapter/ExecutorAdapter.cpp ExecutorAdapter/ExecutorAdapter.h
    State/State.cpp State/State.h
    Strategy/Strategy.cpp Strategy/Strategy.h
    Visitor/Visitor.cpp Visitor/Visitor.h Visitor/StockVisitor.cpp Visitor/StockVisitor.h
    Builder/HtmlBuilder.cpp
    TypeErasure/TypeErasure.cpp TypeErasure/TypeErasure.h
    CopyAndSwap/CopyAndSwap.cpp CopyAndSwap/CopyAndSwap.h
    TagDispatching/TagDispatching.cpp TagDispatching/TagDispatching.h
    Policy/Policy.cpp Policy/Policy.h
    Iterator/Iterator.cpp Iterator/Iterator.h
    Builder/UserBuilder.cpp
    RAII/RAII.cpp RAII/RAII.h
        VirtualConstructor/VirtualConstructor.cpp VirtualConstructor/VirtualConstructor.h State/State.cpp State/State.h TemplateMethod/TemplateMethod.cpp TemplateMethod/TemplateMethod.h Adapter/BaseAdapter.cpp Adapter/GoodExample.cpp Bridge/DrawingBridge.cpp)

TARGET_LINK_LIBRARIES(DesignPatterns pthread)