/**============================================================================
Name        : FactoryMethod_TypeSequence.cpp
Created on  : 31.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FactoryMethod_TypeSequence.cpp
============================================================================**/

#include "FactoryMethod_TypeSequence.hpp"

#include <iostream>
#include <string_view>
#include <thread>
#include <functional>
#include <memory>
#include <utility>

namespace
{

    struct Dinosaur
    {
        virtual ~Dinosaur() = default;
        virtual void info() const = 0;
    };

    struct Diplodocus final : Dinosaur {
        Diplodocus() { std::cout << "Diplodocus::Diplodocus()" << std::endl;}
        void info() const override { std::cout << "Diplodocus" << std::endl;}
    };

    struct Stegosaurus final : Dinosaur {
        Stegosaurus() { std::cout << "Stegosaurus::Stegosaurus()" << std::endl;}
        void info() const override { std::cout << "Stegosaurus" << std::endl; }
    };

    struct Tyrannosaurus final : Dinosaur {
        Tyrannosaurus() { std::cout << "Tyrannosaurus::Tyrannosaurus()" << std::endl;}
        void info() const override { std::cout << "Tyrannosaurus" << std::endl; }
    };

    struct TyrannosaurusEx final : Dinosaur {
        void info() const override {
            std::cout << "Tyrannosaurus" << std::endl;
        }
    };

    template <class... T>
    struct type_sequence {};

    using dinosaur_types = type_sequence<
        Diplodocus,
        Stegosaurus,
        Tyrannosaurus
        // , TyrannosaurusEx
    >;

    using namespace std::string_view_literals;

    template <class T>
    consteval std::string_view getClassName() = delete("Disallow resolution");

    template <class T>
    bool make_dino_impl(std::unique_ptr<Dinosaur>& out, std::string_view name) {
        if (name == getClassName<T>()) {
            out = std::make_unique<T>();
            return true;
        }
        return false;
    }

    template <>
    consteval std::string_view getClassName<Diplodocus>() {
        return "Diplodocus"sv;
    }

    template <>
    consteval std::string_view getClassName<Stegosaurus>() {
        return "Stegosaurus"sv;
    }

    template <>
    consteval std::string_view getClassName<Tyrannosaurus>() {
        return "Tyrannosaurus"sv;
    }



    template <class... Ts>
    constexpr std::unique_ptr<Dinosaur> make_dinosaur_from(type_sequence<Ts...>, const std::string_view name)
    {
        std::unique_ptr<Dinosaur> ptr;
        (make_dino_impl<Ts>(ptr, name) || ...); // for each type in Ts ...
        return ptr;
    }

    template <class T>
    std::unique_ptr<Dinosaur> make_unique_dino() {
        return std::make_unique<T>();
    }

    template <class... Ts>
    std::unique_ptr<Dinosaur> make_dinosaur_from_map(type_sequence<Ts...>, const std::string_view name)
    {
        static std::unordered_map dinosaur_map {
            std::pair { getClassName<Ts>(), &make_unique_dino<Ts> } ...
        };
        if (auto it = dinosaur_map.find(std::string(name)); it != dinosaur_map.end()) {
            auto& fn = it->second;
            return fn();
        }
        return nullptr;
    }


    constexpr std::unique_ptr<Dinosaur> make_dinosaur(const std::string_view name) {
        return make_dinosaur_from(dinosaur_types{}, name);
    }

    constexpr std::unique_ptr<Dinosaur> make_dinosaur_map(const std::string_view name) {
        return make_dinosaur_from_map(dinosaur_types{}, name);
    }
}


void FactoryMethod_TypeSequence::TestAll()
{
    constexpr std::string_view name = "Stegosaurus";

#if 1
    {
        std::unique_ptr<Dinosaur> obj = make_dinosaur(name);
        obj->info();
    }
#else

{
    std::unique_ptr<Dinosaur> obj = make_dinosaur_map(name);
    obj->info();
}
#endif
}