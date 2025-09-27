/**============================================================================
Name        : TypeErasure.cpp
Created on  : 08.05.22.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeErasure
============================================================================**/

#include "TypeErasure.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>

namespace TypeErasure::ExperimentFirst
{
    struct Object
    {
        template<typename T>
        explicit Object(T&& obj):
            concept_(std::make_shared<struct ConcreteCommand<T>>(std::forward<T>(obj))) {
        }

        [[nodiscard]]
        int get_id() const { return concept_->get_id(); }

    private:

        struct ICommand
        {
            virtual ~ICommand() = default;

            [[nodiscard]]
            virtual int get_id() const = 0;
        };

        template<typename T>
        struct ConcreteCommand final : ICommand
        {
            explicit ConcreteCommand(T&& obj) noexcept : object_(std::forward<T>(obj)) {
            }

            [[nodiscard]]
            int get_id() const final {
                return object_.get_id();
            }

        private:
            T object_;
        };

        std::shared_ptr<ICommand> concept_;
    };

    class Bar {
    public:
        [[nodiscard]]
        int get_id() const { return 1; }
    };

    struct Foo {
    public:
        [[nodiscard]]
        int get_id() const { return 2; }
    };

    struct FooEx {
    public:
        // [[nodiscard]]
        // int get_id() const { return 2; }
    };

    void Test()
    {
        Object o1(Bar{});
        Object o2(Foo{});
        // Object o3(FooEx{});

        std::cout << o1.get_id() << std::endl;
        std::cout << o2.get_id() << std::endl;
    }
}

namespace TypeErasure::ExperimentSecond
{
    template<typename T>
    concept HasGetNameMethod = requires(T & a) {
        { a.getName()  } -> std::same_as<std::string>;
    };

    struct Object
    {
        struct Interface
        {
            virtual ~Interface() = default;

            [[nodiscard]]
            virtual std::string getName() const = 0;
        };

        template<HasGetNameMethod T>
        struct Model : Interface
        {
            explicit Model(const T& t) : object { t } { }
            explicit Model(T&& t) : object { std::move(t) } { }

            [[nodiscard]]
            std::string getName() const override {
                return object.getName();
            }
        private:
            T object;
        };

        std::shared_ptr<const Interface> object {};

        template <typename T>
        explicit Object(T&& obj): object {
            std::make_shared<Model<T>>(std::forward<T>(obj))} {
        }

        [[nodiscard]]
        std::string getName() const {
            return object->getName();
        }
    };

    struct Bar {
        [[nodiscard]]
        static std::string getName() {
            return "Bar";
        }
    };

    struct Foo {
        [[nodiscard]]
        std::string getName() const {
            return "Foo";
        }
    };

    void Test()
    {
        std::vector<Object> vec{Object(Foo()), Object(Bar())};  // (1)
        for (auto v: vec)
            std::cout << v.getName() << '\n';
    }
}


namespace TypeErasure::TypeErasure_VoidType
{
    // Concept representing the interface (optional):
    template <typename T>
    concept Interface = requires (T t) {
        { t.operation() } -> std::same_as<int>;
    };

    // Owning variant of a generic holder
    struct GenericHolder
    {
        // Only the constructor is specific to each type:
        template<Interface T>
        explicit GenericHolder(std::unique_ptr<T> ptr)
        {
            // operation_ and destroy_ remember the type
            operation_ = [](void* blob) {
                return static_cast<T*>(blob)->operation();
            };
            destroy_ = [](void* blob) {
                delete static_cast<T*>(blob);
            };
            blob_ = ptr.release();
        }

        ~GenericHolder()
        {
            if (blob_)
                destroy_(this->blob_);
        }

        // Move only (can be made copyable by addition of a clone_ fp)
        GenericHolder(const GenericHolder&) = delete;
        GenericHolder& operator=(const GenericHolder&) = delete;

        // Move operations
        GenericHolder(GenericHolder&& other) noexcept :
                blob_(std::exchange(other.blob_, nullptr)),
                operation_(std::exchange(other.operation_, nullptr)),
                destroy_(std::exchange(other.destroy_, nullptr)) {
        }

        GenericHolder& operator=(GenericHolder&& other) noexcept {
            blob_ = std::exchange(other.blob_, nullptr);
            operation_ = std::exchange(other.operation_, nullptr);
            destroy_ = std::exchange(other.destroy_, nullptr);
            return *this;
        }

        // Actual interface
        int operation() { return operation_(this->blob_); }

    private:
        // Generic storage, note that adding a new operation breaks ABI
        void *blob_;
        int (*operation_)(void*);
        void (*destroy_)(void*);
    };

    // Implementations are unrelated and have no virtual methods
    struct ImplA {
        [[nodiscard]]
        int operation() const { return rank; }

        int rank;
    };

    struct ImplB {
        [[nodiscard]]
        int operation() const { return std::stoi(text); }

        std::string text;
    };

    void user(GenericHolder data) {
        int v = data.operation();
        std::cout << v << "\n";
    }

    void test()
    {
        user(GenericHolder(std::make_unique<ImplA>(10))); // OK, prints 10
        user(GenericHolder(std::make_unique<ImplB>("42"))); // OK, prints 42
    }
}

void TypeErasure_CallOperator_Test();

void TypeErasure::Test()
{
    // ExperimentFirst::Test();
    // TypeErasure_Two::Test();
    TypeErasure_Simple::Test();

    // ExperimentSecond::Test();


    // TypeErasure_VoidType::test();

    // TypeErasure_CallOperator_Test();
};

