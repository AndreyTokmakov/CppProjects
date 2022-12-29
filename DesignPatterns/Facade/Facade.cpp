//============================================================================
// Name        : Facade.cpp
// Created on  : 20.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Facade pattern src
//============================================================================

#include "Facade.h"
#include <iostream>
#include <string>
#include <memory>

namespace Facade {

	/**
	 * The Subsystem can accept requests either from the facade or client directly.
	 * In any case, to the Subsystem, the Facade is yet another client, and it's not
	 * a part of the Subsystem.
	 */
	std::string System1::Operation1() const noexcept {
		return "Subsystem1: Ready!\n";
	}

	std::string System1::OperationN() const noexcept {
		return "Subsystem1: Go!\n";
	}



	/**
	 * Some facades can work with multiple subsystems at the same time.
	 */
	std::string System2::Operation1() const noexcept {
		return "Subsystem2: Get ready!\n";
	}

	std::string System2::OperationZ() const noexcept {
		return "Subsystem2: Fire!\n";
	}



	/**
	 * In this case we will delegate the memory ownership to Facade Class
	 */
	Facade::Facade(std::unique_ptr<System1> sys1, std::unique_ptr<System2> sys2):
		system1(std::move(sys1)), system2(std::move(sys2)) {
	}

	std::string Facade::Operation() noexcept {
		std::string result = "Facade initializes subsystems:\n";
		result += this->system1->Operation1();
		result += this->system2->Operation1();
		result += "Facade orders subsystems to perform the action:\n";
		result += this->system1->OperationN();
		result += this->system2->OperationZ();
		return result;
	}
}

namespace Facade::HouseFacade
{
    struct Alarm
    {
        void alarmOn() {
            std::cout << "Alarm is on and house is secured"<<std::endl;
        }

        void alarmOff() {
            std::cout << "Alarm is off and you can go into the house"<<std::endl;
        }
    };

    struct Ac
    {
        void acOn() {
            std::cout << "Ac is on"<<std::endl;
        }

        void acOff() {
            std::cout << "AC is off"<<std::endl;
        }
    };

    struct Tv
    {
        void tvOn() {
            std::cout << "Tv is on"<<std::endl;
        }

        void tvOff() {
            std::cout << "TV is off"<<std::endl;
        }
    };

    struct HouseFacade
    {
        Alarm alarm;
        Ac ac;
        Tv tv;

        void goToWork() {
            ac.acOff();
            tv.tvOff();
            alarm.alarmOn();
        }

        void comeHome() {
            alarm.alarmOff();
            ac.acOn();
            tv.tvOn();
        }
    };

    void Test()
    {
        HouseFacade hf;

        hf.goToWork();
        hf.comeHome();
    }
};

void Facade::Test()
{
	// std::shared_ptr<Facade> facade = std::make_shared<Facade>(std::unique_ptr<System1>(), std::unique_ptr<System2>());
	// std::cout << facade->Operation();

    HouseFacade::Test();
}
