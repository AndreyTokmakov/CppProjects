//============================================================================
// Name        : Proxy.cpp
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Proxy src
//============================================================================

#include "Proxy.h"

namespace Proxy_Pattern_Tests {

	ICar::~ICar() {
		std::cout << "ICar destructor!" << std::endl;
	}


	void Car::DriveCar() noexcept {
		std::cout << "Car has been driven!" << std::endl;
	}



	ProxyCar::ProxyCar(int driver_age) : driver_age_(driver_age) {
	}

	void ProxyCar::DriveCar() noexcept {
		if (this->driver_age_ > 16) {
			this->real_car_->DriveCar();
		}
		else {
			std::cout << "Sorry, the driver is too young to drive." << std::endl;
		}
	}
}