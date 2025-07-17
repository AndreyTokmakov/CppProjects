//
// Created by andtokm on 10/11/24.
//

#include "AccessToken.h"
#include <iostream>
#include <memory>


/// Паттерн позволят предоставить для std::make_shared публичный конструктор, но который можно вызвать,
/// только имея экземпляр приватного типа (access token)
/// То есть не имея экземепляра класс вызвать ПУБЛИЧНЫЙ контстуктор будет нелья

namespace AccessToken
{
    struct Arg1 {};
    struct Arg2 {};

    class MyComponent
    {
        struct private_ctor_token {
            explicit private_ctor_token() = default;
        };

    public:
        static std::shared_ptr<MyComponent> make(Arg1 arg1, Arg2 arg2) {
            return std::make_shared<MyComponent>(private_ctor_token{}, arg1, arg2);
        }

        MyComponent(const MyComponent&) = delete;
        MyComponent(MyComponent&&) = delete;
        MyComponent& operator = (const MyComponent&) = delete;
        MyComponent& operator = (MyComponent&&) = delete;

        MyComponent(private_ctor_token, Arg1, Arg2)
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };
    };

}

void AccessToken::TestAll()
{
    auto obj = MyComponent::make(Arg1{}, Arg2{});

    /// Will Not Compile
    // auto obj1 = MyComponent(MyComponent::private_ctor_token{}, Arg1{}, Arg2{});
    // auto obj2 = MyComponent({}, Arg1{}, Arg2{});

    //auto obj4 = MyComponent(obj, Arg1{}, Arg2{});
}