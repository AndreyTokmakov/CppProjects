//============================================================================
// Name        : Subject.h
// Created on  : 30.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Proxy src
//============================================================================

#ifndef PROXY_PATTERN_TESTS_SUBJECT__H_
#define PROXY_PATTERN_TESTS_SUBJECT__H_

#include <iostream>
#include <memory>

namespace Proxy_Pattern_Tests::Subject {

	/**
	 * The Subject interface declares common operations for both RealSubject and the
	 * Proxy. As long as the client works with RealSubject using this interface,
	 * you'll be able to pass it a proxy instead of a real subject.
	 */
	class Subject {
	public:
		virtual void Request() const = 0;
	};


	/**
	 * The RealSubject contains some core business logic. Usually, RealSubjects are
	 * capable of doing some useful work which may also be very slow or sensitive -
	 * e.g. correcting input data. A Proxy can solve these issues without any
	 * changes to the RealSubject's code.
	 */
	class RealSubject : public Subject {
	public:
		void Request() const override;
	};


	/**
	 * The Proxy has an interface identical to the RealSubject.
	 */
	class Proxy : public Subject {
	private:
		std::shared_ptr<RealSubject> realSubject;

		bool CheckAccess() const;
		void LogAccess() const;
		/**
		 * The Proxy maintains a reference to an object of the RealSubject class. It
		 * can be either lazy-loaded or passed to the Proxy by the client.
		 */
	public:
		Proxy(std::shared_ptr<RealSubject> realSubject);
		~Proxy() = default;

		/**
		 * The most common applications of the Proxy pattern are lazy loading,
		 * caching, controlling the access, logging, etc. A Proxy can perform one of
		 * these things and then, depending on the result, pass the execution to the
		 * same method in a linked RealSubject object.
		 */
		void Request() const override;
	};


	void Test();
}

#endif /* PROXY_PATTERN_TESTS_SUBJECT__H_ */