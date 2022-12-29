//============================================================================
// Name        : StockVisitor.cpp
// Created on  : 14.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Visitor pattern src
//============================================================================

#include "StockVisitor.h"

#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace StockVisitor {

	int Visitor::m_num_apple = 0;
	int Visitor::m_num_google = 0;

	void Apple::accept(std::shared_ptr<Visitor> visitor) {
		visitor->visit(this->shared_from_this());
	}

	void Apple::buy() {
		std::cout << "Apple::buy" << std::endl;
	}

	void Apple::sell() {
		std::cout << "Apple::sell" << std::endl;
	}


	void Google::accept(std::shared_ptr<Visitor> visitor) {
		visitor->visit(this->shared_from_this());
	}

	void Google::buy() {
		std::cout << "Apple::buy" << std::endl;
	}

	void Google::sell() {
		std::cout << "Apple::sell" << std::endl;
	}



	void Visitor::total_stocks() {
		std::cout << "m_num_apple " << m_num_apple << ", m_num_google " << m_num_google << std::endl;
	}


	BuyVisitor::BuyVisitor() {
		m_num_apple = m_num_google = 0;
	}

	void BuyVisitor::visit(std::shared_ptr<Apple> stock) {
		m_num_apple++;
		stock->buy();
		std::cout << "m_num_apple " << m_num_apple << std::endl;
	}

	void BuyVisitor::visit(std::shared_ptr<Google> stock) {
		m_num_google++;
		stock->buy();
		std::cout << " m_num_google " << m_num_google << std::endl;
	}

	/*
	SellVisitor::SellVisitor()
	{
	}
	*/

	void SellVisitor::visit(std::shared_ptr<Apple> stock) {
		m_num_apple--;
		stock->sell();
		std::cout << "m_num_apple " << m_num_apple << std::endl;
	}

	void SellVisitor::visit(std::shared_ptr<Google> stock) {
		m_num_google--;
		stock->sell();
		std::cout << "m_num_google " << m_num_google << std::endl;
	}



	void test()
	{

		std::vector<std::shared_ptr<Stock>> stocks;

		stocks.push_back(std::make_shared<Apple>());
		stocks.push_back(std::make_shared<Google>());
		stocks.push_back(std::make_shared<Google>());
		stocks.push_back(std::make_shared<Apple>());
		stocks.push_back(std::make_shared<Apple>());

		std::shared_ptr<Visitor> buy = std::make_shared<BuyVisitor>();
		for (auto stock : stocks)
			stock->accept(buy);
		buy->total_stocks();


		std::shared_ptr<Visitor> sell = std::make_shared<SellVisitor>();
		for (auto stock : stocks)
			stock->accept(sell);
		sell->total_stocks();

	}
}