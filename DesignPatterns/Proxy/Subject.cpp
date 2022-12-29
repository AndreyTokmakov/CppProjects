//============================================================================
// Name        : Subject.cpp
// Created on  : 30.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Proxy src
//============================================================================

#include "Subject.h"

namespace Proxy_Pattern_Tests::Subject {

	void RealSubject::Request() const
	{
		std::cout << "RealSubject: Handling request." << std::endl;
	}


	Proxy::Proxy(std::shared_ptr<RealSubject> realSubject): realSubject(realSubject) {
		// Do something more maybe,
	}

	bool Proxy::CheckAccess() const {
		// Some real checks should go here.
		std::cout << "  Proxy: Checking access prior to firing a real request." << std::endl;
		return true;
	}

	void Proxy::LogAccess() const {
		std::cout << "  Proxy: Logging the time of request." << std::endl;;
	}

	void Proxy::Request() const {
		if (this->CheckAccess()) {
			this->realSubject->Request();
			this->LogAccess();
		}
	}


	void Test() {
		std::cout << "Client: Executing the client code with a real subject:\n";
		std::shared_ptr<RealSubject> realSubject = std::make_shared<RealSubject>();

		realSubject->Request();

		std::cout << "-----------------" << std::endl;

		std::cout << "Client: Executing the same client code with a proxy:\n";
		Proxy *proxy = new Proxy(realSubject);

		proxy->Request();
	}
}
