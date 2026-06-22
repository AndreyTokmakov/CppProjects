cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DesignPatterns)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

# include all components
add_executable(${PROJECT_NAME}
		main.cpp
		AbstractFactory/AbstractFactory.cpp
		AbstractFactory/ShapeFactory.cpp
		Adapter/Adapter.cpp
		Adapter/USBAdapter.cpp
		Bridge/Bridge.cpp
		Builder/Builder.cpp
		Builder/BlochBuilder.cpp
		ChainOfResponsibility/ChainOfResponsibility.cpp
		ChainOfResponsibility/ChainOfHandlers.cpp

		Command/Command.cpp
		Command/Command_MethodPtr.cpp
		Command/FilesystemCommandDispatcher.cpp
		Command/Command_CRTP.cpp

		Composite/Composite.cpp
		CRTP/CRTP.cpp
		CRTP/CRTP.h
		CRTP/ObjectCounter.cpp
		CRTP/CRTP_InvokeMethods.cpp
		CRTP/Policy_Based_Design.cpp
		CRTP/Simple_Comparable_Base.cpp
		CRTP/CRTP_Cloneable.cpp
		Decorator/Decorator.cpp
		FabricMethod/FabricMethod.h
		FabricMethod/FabricMethod.cpp
		FabricMethod/FabricMethod2.cpp

		FactoryMethod_TypeSequence/FactoryMethod_TypeSequence.cpp

		Factory_Registry/Factory_Registry_Basic.cpp
		Factory_Registry/Factory_Registry_CompileTime.cpp

		Facade/Facade.cpp Facade/Facade.h
		Filter/Filter.cpp Filter/Filter.h Filter/Filter_ColorSize.cpp
		Flyweight/Flyweight.cpp Flyweight/Flyweight.h Flyweight/IconFlyweight.cpp
		Interpreter/Interpreter.cpp Interpreter/Interpreter.h
		Mediator/Mediator.cpp Mediator/Mediator.h
		Memento/Memento.cpp Memento/Memento.h
		Named_Parameter_Idiom/NamedParameter.cpp Named_Parameter_Idiom/NamedParameter.h
		ObjectPools/ObjectPool.cpp ObjectPools/ObjectPoolsExtended.cpp ObjectPools/ObjectPools.h
		Observer/Observer.cpp Observer/Observer.h
		Prototype/Prototype.cpp Prototype/Prototype.h
		Proxy/Document.cpp Proxy/CarProxy.cpp Proxy/Proxy.h
		Prototype/PrototypeDemoTwo.cpp
		Proxy/Subject.cpp
		Proxy/DatabaseProxy.cpp
		RAII/RAII.cpp
		Multiton/Multiton.cpp
		Singleton/Singleton.cpp
		Singleton/SingletonCRTP.cpp
		ExecutorAdapter/ExecutorAdapter.cpp
		State/State.cpp
		Strategy/Strategy.cpp
		Visitor/Visitor.cpp
		Visitor/StockVisitor.cpp
		Visitor/DynamicComponentsVisitor.cpp
		Builder/HtmlBuilder.cpp

		TypeErasure/TypeErasure.cpp
		TypeErasure/TypeErasure_CallOperator.cpp
		TypeErasure/TypeErasure_Simple.cpp
		TypeErasure/TypeErasure_Two.cpp
		TypeErasure/Function.cpp

		CopyAndSwap/CopyAndSwap.cpp
		TagDispatching/TagDispatching.cpp
		TagDispatching/TagDispatching_RegisterIO.cpp
		Policy/Policy.cpp
		Policy/Policy.cpp
		Policy/CheckingPolicy.cpp
		Iterator/Iterator.cpp
		Builder/UserBuilder.cpp
		RAII/RAII.cpp
		VirtualConstructor/VirtualConstructor.cpp
		TemplateMethod/TemplateMethod.cpp
		Adapter/BaseAdapter.cpp Adapter/GoodExample.cpp
		Bridge/DrawingBridge.cpp
		Bridge/DrawingBridgeTwo.cpp
		Decorator/ShapeDecorator.cpp
		LazyInitialization/LazyInitialization.cpp
		Lazy_Computation/LazyComputation.cpp
		ChainOfResponsibility/Example2.cpp
		Iterator/RangeIterator.cpp
		Mediator/ChatMediator.cpp
		Observer/ObserverBasic.cpp
		TemplateMethod/TemplateMethod_ExampleTwo.cpp
		Observer/ObserverMediator.cpp
		TemplateMethod/TemplateMethod_ExampleThree.cpp
		Visitor/DynamicComponentsVisitor.cpp
		Visitor/DynamicComponentsVisitor.cpp
		Visitor/DynamicComponentsVisitor.cpp
		Observer/ObserverSubject.cpp
		Visitor/CarVisitor.cpp
		PImpl/PImpl.cpp
		PImpl/PImplDemo2.cpp
		Monostate/Monostate.cpp
		NVI/NVI_SimpleExample.cpp
		Decorator/MoneyTaxesDecoratorOne.cpp
		Decorator/MoneyTaxesDecoratorTwo.cpp
		Observer/LambdaObserver.cpp
		MVC/ModelViewController.cpp
		DependencyInjection/DependencyInjection.cpp
		Adapter/ChargingPhoneAdapter.cpp
		Adapter/ChargingPhoneAdapter.cpp
		Decorator/WindowDecorator.cpp
        TypeID/TypeID.cpp
		AccessToken/AccessToken.cpp
		Passkey_Idiom/PasskeyIdiom.cpp
		Passkey_Idiom/PasskeyTemplate.cpp
		FactoryClasses/FactoryClasses.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
)