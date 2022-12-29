//============================================================================
// Name        : StockVisitor.h
// Created on  : 14.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Visitor pattern src
//============================================================================

#ifndef STOCK_VISITOR_PATTERN_TESTS__H_
#define STOCK_VISITOR_PATTERN_TESTS__H_

#include <iostream>
#include <string>
#include <memory>

namespace StockVisitor {

	class Stock {
	public:
		virtual void accept(std::shared_ptr<class Visitor> visitor) = 0;
	};

	class Apple : public Stock, public std::enable_shared_from_this<Apple> {
	public:
		virtual void accept(std::shared_ptr<Visitor> visitor) override;
		void buy();
		void sell();
	};

	class Google : public Stock, public std::enable_shared_from_this<Google> {
	public:
		virtual void accept(std::shared_ptr<Visitor> visitor) override;
		void buy();
		void sell();
	};



	class Visitor {
	protected:
		static int m_num_apple, m_num_google;;

	public:
		virtual void visit(std::shared_ptr<Apple> stock) = 0;
		virtual void visit(std::shared_ptr<Google> stock) = 0;

		void total_stocks();
	};

	class BuyVisitor : public Visitor {
	public:
		BuyVisitor();

		virtual void visit(std::shared_ptr<Apple> stock) override;
		virtual void visit(std::shared_ptr<Google> stock) override;
	};

	class SellVisitor : public Visitor {
	public:
		//SellVisitor();

		virtual void visit(std::shared_ptr<Apple> stock) override;
		virtual void visit(std::shared_ptr<Google> stock) override;
	};


	//** The entry point here. **/
	void test();
};

#endif // !STOCK_VISITOR_PATTERN_TESTS__H_
