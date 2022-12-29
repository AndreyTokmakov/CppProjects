//============================================================================
// Name        : Facade.h
// Created on  : 20.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Facade pattern src
//============================================================================

#ifndef FACADE_DESIGN_PATTERN_TESTS__H_
#define FACADE_DESIGN_PATTERN_TESTS__H_

#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace Facade {

	/**
	 * The Subsystem can accept requests either from the facade or client directly.
	 * In any case, to the Subsystem, the Facade is yet another client, and it's not
	 * a part of the Subsystem.
	 */
	class System1 {
	public:
		std::string Operation1() const noexcept;
		std::string OperationN() const noexcept;
	};



	/**
	 * Some facades can work with multiple subsystems at the same time.
	 */
	class System2 {
	public:
		std::string Operation1() const noexcept;
		std::string OperationZ() const noexcept;
	};


	/**
	 * The Facade class provides a simple interface to the complex logic of one or
	 * several subsystems. The Facade delegates the client requests to the
	 * appropriate objects within the subsystem. The Facade is also responsible for
	 * managing their lifecycle. All of this shields the client from the undesired
	 * complexity of the subsystem.
	 */
	class Facade {
	protected:
		std::unique_ptr<System1> system1;
		std::unique_ptr<System2> system2;
		/**
		 * Depending on your application's needs, you can provide the Facade with
		 * existing subsystem objects or force the Facade to create them on its own.
		 */
	public:
		/**
		 * In this case we will delegate the memory ownership to Facade Class
		 */
		Facade(std::unique_ptr<System1> sys1,
			   std::unique_ptr<System2> sys2);
		virtual ~Facade() = default;
		/**
		 * The Facade's methods are convenient shortcuts to the sophisticated
		 * functionality of the subsystems. However, clients get only to a fraction of
		 * a subsystem's capabilities.
		 */
		std::string Operation() noexcept;
	};

};

namespace Facade {
	void Test();
};

#endif // !FACADE_DESIGN_PATTERN_TESTS__H_