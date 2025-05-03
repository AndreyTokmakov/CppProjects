/**============================================================================
Name        : CRTP.cpp
Created on  : 09.04.2021
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP src
============================================================================**/

#include <iostream>
#include <memory>
#include <vector>

#include "CRTP.h"
#include "ObjectCounter.h"
#include "CRTP_Cloneable.h"


namespace CRTP::SimpleExample_One
{
    template <typename Derived>
    struct Base
    {
        void info() const {
            self().info();
        }

        [[nodiscard]]
        inline const Derived &self() const noexcept {
            return *static_cast<const Derived *const>(this);
        }
    };

    struct DerivedTypeOne: Base<DerivedTypeOne> {
        void info() const {
            std::cout << "DerivedTypeOne::info()\n";
        }
    };

    struct DerivedTypeTwo: Base<DerivedTypeTwo> {
        void info() const {
            std::cout << "DerivedTypeTwo::info()\n";
        }
    };

    template<typename Derived>
    void print(const Base<Derived>& obj)
    {
        obj.info();
    }

    void test()
    {
        print(DerivedTypeOne{});
        print(DerivedTypeTwo{});
    }
}

namespace CRTP::SimpleExample_Two
{
    template<typename Derived>
    struct Animal
    {
        void make_sound() const {
            const Derived& underlying = static_cast<const Derived&>(*this);
            std::cout << "Animal::make_sound()" << std::endl;
            underlying.make_sound();
        }
    };

    struct Cow: public Animal<Cow> {
        void make_sound() const { std::cout << "moo\n"; }
    };

    struct Sheep: public Animal<Sheep> {
        void make_sound() const { std::cout << "baa\n"; }
    };

    struct Dog: public Animal<Dog> {
        void make_sound() const { std::cout << "wouf\n"; }
    };

    template<typename Derived>
    void print(Animal<Derived> const& animal) {
        animal.make_sound();
    }

    void test()
    {
        print(Cow{});
        print(Sheep{});
        print(Dog{});
    }
}

namespace CRTP::SimpleExample_Three
{
	template <typename Derived>
    struct Base
	{
		using underlying_type = Derived;

		inline constexpr underlying_type& underlying() noexcept {
			return static_cast<underlying_type&>(*this);
		}

		inline constexpr const underlying_type& underlying() const noexcept {
			return static_cast<const underlying_type&>(*this);
		}

	public:
		void info() {
			underlying().info_impl();
		}

	protected:
		virtual void info_impl() = 0;
	};


	struct Triangle : public Base<Triangle>
    {
		void info_impl() override {
			std::cout << "Triangle::info_impl()\n";
		}
	};

    struct Square : public Base<Square>
    {
		void info_impl() override {
			std::cout << "Square::info_impl()\n";
		}
	};

    void test1()
    {
        Triangle().info();
        Square().info();
    }
}

namespace CRTP::Cloneable
{
    struct ICloneable {
        virtual ~ICloneable() = default;

        [[nodiscard]]
        virtual std::unique_ptr<ICloneable> clone() const = 0;
    };

    template <typename Self>
    class clone_inherit: public ICloneable
    {
    public:
        [[nodiscard]]
        std::unique_ptr<ICloneable> clone() const override {
            return std::unique_ptr<Self>(static_cast<Self*>(this->clone_impl()));
        }

    private:

        virtual clone_inherit* clone_impl() const {
            return new Self(*this);
        }
    };


    /*
    class cloneable
    {
    public:
        virtual ~cloneable() = default;

        [[nodiscard]]
        virtual std::unique_ptr<cloneable> clone() const = 0;

    private:

        [[nodiscard]]
        virtual cloneable * clone_impl() const = 0;
    };

    class concrete: public clone_inherit<concrete, cloneable>
    {
    };
    */
}

void CRTP_InvokeMethods_Test();

void CRTP::Test()
{
    // SimpleExample_One::test();
    // SimpleExample_Two::test();
    // SimpleExample_Three::test1();

    // ObjectCounter::Test();

    // CRTP_Cloneable::Test();

    CRTP_InvokeMethods_Test();
}