/**============================================================================
Name        : Interpreter.cpp
Created on  : October 23, 2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Interpreter pattern test
===========================================================================**/

#include "Interpreter.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <initializer_list>

namespace
{
	struct IExpression
	{
		virtual ~IExpression() = default;

		[[nodiscard]]
	    virtual bool interpret(const std::string& condition) const noexcept = 0;
	};

	struct Expression : IExpression
	{
		std::string str;

		explicit Expression(std::string data): str(std::move(data)){
		}

		[[nodiscard]]
	    bool interpret(const std::string& condition) const noexcept override {
			return std::string::npos != condition.find(this->str);
		}
	};

	class ComplexExpression : public IExpression
	{
	public:
		ComplexExpression(const std::initializer_list<std::shared_ptr<IExpression>>& il) {
			exprList.assign(il);
		}

		void addExpression(const std::shared_ptr<IExpression>& expr) {
			exprList.push_back(expr);
		}

		[[nodiscard]]
	    bool interpret(const std::string& condition) const noexcept override = 0;

	protected:
		std::vector<std::shared_ptr<IExpression>> exprList;
	};

	struct OrExpression: public ComplexExpression
	{
		OrExpression(const std::initializer_list<std::shared_ptr<IExpression>>& il):
			ComplexExpression(il){
		}

		[[nodiscard]]
	    bool interpret(const std::string& condition) const noexcept override {
			return std::ranges::any_of(exprList, [&condition](const auto& expr) {
				return expr->interpret(condition);
			});
		}
	};

	class AndExpression : public ComplexExpression
	{
	public:
		AndExpression(const std::initializer_list<std::shared_ptr<IExpression>>& il) :
			ComplexExpression(il) {
		}

		[[nodiscard]]
	    bool interpret(const std::string& condition) const noexcept override {
			return std::ranges::all_of(exprList, [&condition](const auto& expr) {
				return expr->interpret(condition);
			});
		}
	};
}


void interpreter::TestAll()
{
	/*
	const auto expr1 = std::make_shared<Expression>("AAA");
	const auto expr2 = std::make_shared<Expression>("BBB");

	OrExpression orExpr({ expr1 , expr2 });
	AndExpression andExpr({ expr1 , expr2 });

	std::cout << std::boolalpha << orExpr.interpret("AAABBCCDD")  << std::endl;  // true
	std::cout << std::boolalpha << andExpr.interpret("AAACCCBBB") << std::endl;  // true

	std::cout << std::boolalpha << orExpr.interpret("AAA")  << std::endl;       // true
	std::cout << std::boolalpha << andExpr.interpret("AAABBCCDD") << std::endl; // false
	*/

	interpreter_dsl::TestAll();
};

