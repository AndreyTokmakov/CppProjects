//============================================================================
// Name        : Events.h
// Created on  : 25.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Events
//============================================================================

#include "Events.h"

#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"

#include <iostream>

namespace Events
{
    class Source {
    public:
        Poco::BasicEvent<int> theEvent;

        void fireEvent(int n) {
            theEvent(this, n);
        }
    };

    class Target {
    public:
        void onEvent([[maybe_unused]] const void* pSender, int& arg) {
            std::cout << "onEvent: " << arg << std::endl;
        }
    };

    void DelegateTest() {
        Source source;
        Target target;

        source.theEvent += Poco::delegate(&target, &Target::onEvent);
        source.fireEvent(42);
        source.theEvent -= Poco::delegate(&target, &Target::onEvent);
    }
}

void Events::TestAll() {
    DelegateTest();
}

