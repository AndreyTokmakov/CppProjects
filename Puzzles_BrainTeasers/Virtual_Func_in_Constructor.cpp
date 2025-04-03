/**============================================================================
Name        : Virtual_Func_in_Constructor.cpp
Created on  : 03.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : False_Start.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    struct GameObject
    {
        GameObject()
        {
            std::cout << "Created a " << getType() << "\n";
        }

        void render() const {
            std::cout << "Rendered a "<< getType() << "\n";
        }

        virtual std::string getType() const {
            return"GameObject";
        }
    };

    class Spaceship : public GameObject
    {
        std::string getType() const override
        {
            return"Spaceship";
        }
    };

    void display(const GameObject &gameObject)
    {
        gameObject.render();
    }
}

namespace
{
    void demo()
    {
        GameObject gameObject;
        Spaceship spaceship;
        display(gameObject);
        display(spaceship);
    }
}

void Puzzles::Virtual_Func_in_Constructor()
{
    demo();
}

/**
Created a GameObject
Created a GameObject
Rendered a GameObject
Rendered a Spaceship
**/