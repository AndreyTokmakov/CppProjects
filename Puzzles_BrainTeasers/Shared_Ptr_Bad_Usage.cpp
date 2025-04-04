/**============================================================================
Name        : Shared_Ptr_Bad_Usage.cpp
Created on  : 04.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Shared_Ptr_Bad_Usage.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <functional>
#include <memory>

namespace
{
    class Wrapper
    {
        std::function<void()> m_handler;

    public:

        Wrapper() {
            std::printf("Wrapper()\n");
        }

        ~Wrapper() {
            std::printf("~Wrapper()\n");
        }

        template<typename F>
        void set_handler(F&& f)
        {
            m_handler = std::forward<F>(f);
        }

        void doAction() const
        {
            std::printf("doAction()\n");
            if (m_handler) {
                m_handler();
            }
        }

        void notify() {
            std::printf("notify()\n");
        }
    };
}

namespace
{
    void demo()
    {
        std::shared_ptr<Wrapper> obj = std::make_shared<Wrapper>();
        obj->set_handler([obj]() {
            obj->notify();
        });

        obj->doAction();

        // Wrapper()
        // doAction()
        // notify()
        // ----> NO: ~Wrapper()
    }

    void fix_1()
    {
        std::shared_ptr<Wrapper> obj = std::make_shared<Wrapper>();
        obj->set_handler([&obj]() {
            obj->notify();
        });

        obj->doAction();

        // Wrapper()
        // doAction()
        // notify()
        // ~Wrapper()
    }

    void fix_2()
    {
        const std::shared_ptr<Wrapper> obj = std::make_shared<Wrapper>();
        obj->set_handler([weak_obj = std::weak_ptr<Wrapper>(obj)]() {
            if (const auto shared = weak_obj.lock(); shared) {
                weak_obj.lock()->notify();
            }
        });

        obj->doAction();

        // Wrapper()
        // doAction()
        // notify()
        // ~Wrapper()
    }
}

void Puzzles::Shared_Ptr_Bad_Usage()
{
    // demo();
    // fix_1();
    fix_2();
}
