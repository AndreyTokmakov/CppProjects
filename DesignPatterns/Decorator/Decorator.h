//============================================================================
// Name        : Decorator.h
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Decorator src
//============================================================================

#ifndef DECORATOR_TESTS__H_
#define DECORATOR_TESTS__H_

#include <iostream>
#include <memory>

class IComponent {
public:
	virtual void operation() = 0;
	virtual ~IComponent() {}
};


class Component : public IComponent {
public:
	virtual void operation();
};


class DecoratorOne : public IComponent {
	std::shared_ptr<IComponent> m_component;

public:
	DecoratorOne(std::shared_ptr<IComponent> component);
	virtual void operation();
};


class DecoratorTwo : public IComponent {
	std::shared_ptr<IComponent> m_component;

public:
	DecoratorTwo(std::shared_ptr<IComponent> component);

	virtual void operation();
};


namespace DecoratorTests {
	void Test();
}

#endif /* DECORATOR_TESTS__H_ */


