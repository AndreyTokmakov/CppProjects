/**============================================================================
Name        : CRTP.cpp
Created on  : 09.04.2021
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP src
============================================================================**/

#include <iostream>
#include "CRTP.h"
#include "ObjectCounter.h"
#include <memory>

namespace CRTP {
	
	template <typename Derived>
	class Base
	{
	private:
		using underlying_type = Derived;

	protected:
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


	class Triangle : public Base<Triangle> {
	public:
		void info_impl() override {
			std::cout << "Triangle::info_impl()\n";
		}
	};

	class Square : public Base<Square> {
	public:
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

void CRTP::Test()
{
    test1();
    // ObjectCounter::Test();
}