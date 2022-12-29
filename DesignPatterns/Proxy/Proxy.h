//============================================================================
// Name        : Proxy.h
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Proxy src
//============================================================================

#ifndef PROXY_PATTERN_TESTS__H_
#define PROXY_PATTERN_TESTS__H_

#include <iostream>
#include <memory>

namespace Proxy_Pattern_Tests {

	class ICar {
	public:
		virtual ~ICar();

		virtual void DriveCar() noexcept = 0;
	};


	class Car : public ICar {
	public:
		void DriveCar() noexcept override;
	};


	class ProxyCar : public ICar {
	private:
		std::unique_ptr<ICar> real_car_ = std::make_unique<Car>();
		int driver_age_;

	public:
		ProxyCar(int driver_age);
		void DriveCar() noexcept override;
	};
}

#endif /* PROXY_PATTERN_TESTS__H_ */