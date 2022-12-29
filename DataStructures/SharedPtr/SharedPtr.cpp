//============================================================================
// Name        : SharedPtr.h
// Created on  : 05.04.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TT LCache src class
//============================================================================

#include "../Utilities/Integer.h"
#include "SharedPtr.h"
#include <iostream>
#include <memory>

namespace Memory::SharedPtr 
{
	template<typename T>
	class DefaultDeleter {
	public:
		DefaultDeleter() { 
			// std::cout << "DefaultDeleter()" << std::endl;
		}

		DefaultDeleter(const DefaultDeleter& deleter) { 
			// std::cout << "DefaultDeleter(const DefaultDeleter& deleter)" << std::endl;
		}

		DefaultDeleter(DefaultDeleter&& deleter) noexcept { 
			// std::cout << "DefaultDeleter(DefaultDeleter&& deleter) noexcept" << std::endl;
		}

		~DefaultDeleter() { 
			// std::cout << "~DefaultDeleter()" << std::endl;
		}

		void operator() (T* ptr) {
			// std::cout << "DefaultDeleter called" << std::endl;
			delete ptr;
		}
	};


	template<typename T, typename Deleter>
	class my_shared_ptr;

	template<typename T, typename Deleter = DefaultDeleter<T>>
	struct ControlBlock {
	private:
		// Use count:
		size_t use_count = 1;

		/** Controlled data pointer: **/
		T* raw_ptr = nullptr;

		/** Deleter: **/
		Deleter _deleter;

	public:
		explicit ControlBlock(T* ptr) : raw_ptr(ptr) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
		}

		ControlBlock(T* ptr, Deleter&& deleter) : raw_ptr(ptr), _deleter(std::move(deleter)) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
		}

		~ControlBlock() {
			if (this->raw_ptr)
				this->_deleter(raw_ptr);
		}

		inline size_t increment_use_count() noexcept {
            return ++use_count;
		}

		inline size_t decrement_use_count() noexcept {
			return --use_count;
		}

		friend class my_shared_ptr<T, Deleter>;
	};

	
	template<typename T, typename Deleter = DefaultDeleter<T>>
	class my_shared_ptr {
	private:
		ControlBlock<T, Deleter>* contr = nullptr;

	public:

		explicit my_shared_ptr(T* ptr) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
			this->contr = new ControlBlock<T, Deleter>(ptr);
		}

		/*
		template<typename ... Params>
		my_shared_ptr(Params&& ... args) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
			this->contr = new ControlBlock<T, Deleter>(new T(std::forward< Params>(args)...));
		}
		*/

		my_shared_ptr(const T* ptr, Deleter&& deleter):
            contr { new ControlBlock<T, Deleter>(ptr, std::move(deleter)) } {
		}

		my_shared_ptr(const my_shared_ptr<T>& obj): contr { obj.contr } {
			contr->increment_use_count();
		}

		my_shared_ptr& operator=(const my_shared_ptr& right) {
			if (&right != this) {
				if (0 == contr->decrement_use_count())
                    delete std::exchange(contr, nullptr);
				contr = right.contr;
				contr->increment_use_count();
			}
			return *this;
		}

        explicit my_shared_ptr(my_shared_ptr<T>&& obj) noexcept:
                contr { std::exchange(obj.contr, nullptr) } {
        }

        my_shared_ptr& operator=(my_shared_ptr&& right) noexcept {
            if (&right != this) {
                if (0 == contr->decrement_use_count())
                    delete std::exchange(contr, nullptr);
                contr = std::exchange(right.contr, nullptr);
            }
            return *this;
        }

		[[nodiscard]]
        T* get() const noexcept {
			return this->contr->raw_ptr;
		}

		T* operator*() const noexcept {
			return this->contr->raw_ptr;
		}

        [[nodiscard]]
        inline size_t UseCount() const noexcept {
            return this->contr->use_count;
        }

		~my_shared_ptr()
        {
			if (nullptr != contr && 0 == contr->decrement_use_count()) {
                delete contr;
            }
		}
	};


	template<typename _Ty, typename ... _Types>
	my_shared_ptr<_Ty> make_shared(_Types&& ... args) {
		return my_shared_ptr<_Ty>(new _Ty(std::forward<_Types>(args)...));
	}

}

namespace Memory::SharedPtr::Tests
{

	void Create_and_Destroy_Ptr() {
		my_shared_ptr<Integer> int1(new Integer(11));
	}


	void Make_Shared_Test() {
		auto var = make_shared<Integer>(1234);
	}

	//---------------------------------------------------------------------

	template<typename T>
	class CustomDeleter {
	public:
		CustomDeleter() {
			std::cout << "CustomDeleter()" << std::endl;
		}

		CustomDeleter(const CustomDeleter& deleter) { 
			std::cout << "CustomDeleter(const CustomDeleter& deleter)" << std::endl; 
		}

		CustomDeleter(CustomDeleter&& deleter) noexcept { 
			std::cout << "CustomDeleter(CustomDeleter&& deleter) noexcept" << std::endl;
		}

		~CustomDeleter() { 
			std::cout << "~CustomDeleter()" << std::endl; 
		}

		void operator() (T* ptr) {
			std::cout << "CustomDeleter called" << std::endl;
			delete ptr;
		}
	};

	template<typename T>
	void handle_shared_ptr(my_shared_ptr<T> ptr) {
		ptr.get()->printInfo();
	}

	void DefaultDeleter_Test() {
		my_shared_ptr<Integer> int1(new Integer(11));
	}


	void CopyConstructor_Test() {
		my_shared_ptr<Integer> a1(new Integer(111));
		my_shared_ptr<Integer> a2 = a1;

		std::cout << a1.get()->getValue() << std::endl;
		std::cout << a2.get()->getValue() << std::endl;
	}

    void CopyAssignment_Test() {
        my_shared_ptr<Integer> a1(new Integer(111)), a2 (new Integer(222));

        a2 = a1;

        std::cout << a1.get()->getValue() << std::endl;
        std::cout << a2.get()->getValue() << std::endl;
    }


	/*
	void CustomDeleter_Test() {
		CustomDeleter<Utilities> deleter;
		my_shared_ptr<Utilities, CustomDeleter<Utilities>> intPtr(new Utilities(123), std::move(deleter));
		// handle_shared_ptr(intPtr);
	}
	*/

    void MoveTest()
    {
        std::cout << "-------------- std::shared_ptr<T> (copy) --------------------------\n";
        {
            std::shared_ptr<Integer> ptr1 = std::make_shared<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.use_count() << std::endl;
            std::shared_ptr<Integer> ptr2 = ptr1;
            std::cout << "use_count(1) = " << ptr1.use_count() << ", use_count(2) = " << ptr2.use_count() << std::endl;
        }
        std::cout << "-------------- my_shared_ptr<T>  (copy)  --------------------------\n";
        {
            my_shared_ptr<Integer> ptr1 = make_shared<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.UseCount() << std::endl;

            my_shared_ptr<Integer> ptr2 = ptr1;
            std::cout << "use_count(1) = " << ptr1.UseCount() << ", use_count(2) = " << ptr2.UseCount() << std::endl;
        }

        std::cout << "-------------- std::shared_ptr<T> (move) --------------------------\n";
        {
            std::shared_ptr<Integer> ptr1 = std::make_unique<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.use_count() << std::endl;
            std::shared_ptr<Integer> ptr2 = std::move(ptr1);
            std::cout << "use_count(1) = " << ptr1.use_count() << ", use_count(2) = " << ptr2.use_count() << std::endl;
        }

        std::cout << "-------------- my_shared_ptr<T>  (move)  --------------------------\n";
        {
            my_shared_ptr<Integer> ptr1 = make_shared<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.UseCount() << std::endl;

            my_shared_ptr<Integer> ptr2 = std::move(ptr1);
            // std::cout << "use_count(1) = " << ptr1.UseCount() << ", use_count(2) = " << ptr2.UseCount() << std::endl;
            std::cout << "use_count(2) = " << ptr2.UseCount() << std::endl;
        }
    }
}


namespace Memory {


	void Shared_Ptr_Tests()
	{
		// SharedPtr::Tests::Create_and_Destroy_Ptr();
		// SharedPtr::Tests::Create_and_Destroy();
		
		// SharedPtr::Tests::Make_Shared_Test();

		// SharedPtr::Tests::CopyConstructor_Test();
		SharedPtr::Tests::CopyAssignment_Test();

        // SharedPtr::Tests::MoveTest();

		// SharedPtr::Tests::DefaultDeleter_Test();

		// SharedPtr::Tests::CustomDeleter_Test();
	}
}

