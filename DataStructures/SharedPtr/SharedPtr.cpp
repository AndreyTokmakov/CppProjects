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
	class SharedPtr;

	template<typename T, typename Deleter = DefaultDeleter<T>>
	struct ControlBlock {
	private:
        using type = T;
        using pointer = type*;

		// Use count:
		size_t use_count = 1;

		/** Controlled data pointer: **/
        pointer raw_ptr = nullptr;

		/** Deleter: **/
		Deleter deleter;

	public:
		explicit ControlBlock(pointer ptr) : raw_ptr(ptr) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
		}

		ControlBlock(pointer ptr, Deleter&& deleter) : raw_ptr(ptr), deleter(std::move(deleter)) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
		}

		~ControlBlock() {
			if (this->raw_ptr)
				this->deleter(raw_ptr);
		}

		inline size_t increment_use_count() noexcept {
            return ++use_count;
		}

		inline size_t decrement_use_count() noexcept {
			return --use_count;
		}

		friend class SharedPtr<type , Deleter>;
	};


	template<typename T, typename Deleter = DefaultDeleter<T>>
	class SharedPtr {
	private:
        using type = T;
        using pointer = type*;

		ControlBlock<type, Deleter> * contr = nullptr;

	public:
		explicit SharedPtr(pointer objectPtr) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
			this->contr = new ControlBlock<type, Deleter>(objectPtr);
		}

		/*
		template<typename ... Params>
		my_shared_ptr(Params&& ... args) {
			// std::cout << __LINE__ << ":" << __FUNCTION__ << std::endl;
			this->contr = new ControlBlock<T, Deleter>(new T(std::forward< Params>(args)...));
		}
		*/

		SharedPtr(const pointer objectPtr, Deleter&& deleter):
            contr { new ControlBlock<T, Deleter>(objectPtr, std::move(deleter)) } {
		}

		explicit SharedPtr(const SharedPtr<type>& obj): contr { obj.contr } {
			contr->increment_use_count();
		}

		SharedPtr& operator=(const SharedPtr& right) {
			if (&right != this) {
				if (0 == contr->decrement_use_count())
                    delete std::exchange(contr, nullptr);
				contr = right.contr;
				contr->increment_use_count();
			}
			return *this;
		}

        explicit SharedPtr(SharedPtr<type>&& obj) noexcept:
                contr { std::exchange(obj.contr, nullptr) } {
        }

        SharedPtr& operator=(SharedPtr&& right) noexcept {
            if (&right != this) {
                if (0 == contr->decrement_use_count())
                    delete std::exchange(contr, nullptr);
                contr = std::exchange(right.contr, nullptr);
            }
            return *this;
        }

		[[nodiscard]]
        pointer get() const noexcept {
			return this->contr->raw_ptr;
		}

        pointer operator*() const noexcept {
			return this->contr->raw_ptr;
		}

        [[nodiscard]]
        inline size_t UseCount() const noexcept {
            return this->contr->use_count;
        }

		~SharedPtr()
        {
			if (nullptr != contr && 0 == contr->decrement_use_count()) {
                delete contr;
            }
		}
	};


	template<typename Ty, typename ... ArgsTypes>
	SharedPtr<Ty> make_shared(ArgsTypes&& ... args) {
		return SharedPtr<Ty>(new Ty(std::forward<ArgsTypes>(args)...));
	}

}

namespace Memory::SharedPtr::Tests
{

	void Create_and_Destroy_Ptr() {
		SharedPtr<Integer> int1(new Integer(11));
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
	void handle_shared_ptr(SharedPtr<T> ptr) {
		ptr.get()->printInfo();
	}

	void DefaultDeleter_Test() {
		SharedPtr<Integer> int1(new Integer(11));
	}


	void CopyConstructor_Test()
    {
		SharedPtr<Integer> a1(new Integer(111));
		SharedPtr<Integer> a2 = a1;

		std::cout << a1.get()->getValue() << std::endl;
		std::cout << a2.get()->getValue() << std::endl;
	}

    void CopyAssignment_Test() {
        SharedPtr<Integer> a1(new Integer(111)), a2 (new Integer(222));

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
            SharedPtr<Integer> ptr1 = make_shared<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.UseCount() << std::endl;

            SharedPtr<Integer> ptr2 = ptr1;
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
            SharedPtr<Integer> ptr1 = make_shared<Integer>(111);
            std::cout << "use_count(1) = " << ptr1.UseCount() << std::endl;

            SharedPtr<Integer> ptr2 = std::move(ptr1);
            // std::cout << "use_count(1) = " << ptr1.UseCount() << ", use_count(2) = " << ptr2.UseCount() << std::endl;
            std::cout << "use_count(2) = " << ptr2.UseCount() << std::endl;
        }
    }
}



void Memory::Shared_Ptr_Tests()
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

