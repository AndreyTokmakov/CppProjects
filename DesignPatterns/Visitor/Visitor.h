//============================================================================
// Name        : Visitor.h
// Created on  : 17.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Visitor pattern src
//============================================================================

#ifndef VISITOR_PATTERN_TESTS__H_
#define VISITOR_PATTERN_TESTS__H_

#include <iostream>
#include <string>
#include <memory>

namespace Visitor_Pattern_Tests {

	class Foo;
	class Bar;
	class Baz;

	class IVisitor {
	public:
		virtual void visit(Foo &ref) = 0;
		virtual void visit(Bar &ref) = 0;
		virtual void visit(Baz &ref) = 0;

		virtual ~IVisitor() = default;
	};

	class IElement {
	public:
		virtual void accept(IVisitor &v) = 0;
		virtual ~IElement() = default;
	};

	class Foo : public IElement {
	public:
		void accept(IVisitor &v) override;
	};
	 
	class Bar : public IElement {
	public:
		void accept(IVisitor &v) override;
	};

	class Baz : public IElement {
	public:
		void accept(IVisitor &v);
	};

	class CustorVisitor : public IVisitor {
	public:
		std::string value;

	public:
		void visit(Foo &ref) override;
		void visit(Bar &ref) override;
		void visit(Baz &ref) override;
	};

	void test();
};

#endif // !VISITOR_PATTERN_TESTS__H_