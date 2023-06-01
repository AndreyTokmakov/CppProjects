/**============================================================================
Name        : ChargingPhoneAdapter.cpp
Created on  : 31.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ChargingPhoneAdapter
============================================================================**/

#include "Adapter.h"
#include <memory>
#include <utility>

namespace Adapter
{

    struct ILightningPhone {
        virtual void recharge() = 0;
        virtual void useLightning() = 0;

        virtual ~ILightningPhone() = default;
    };

    struct IMicroUsbPhone {
        virtual void recharge() = 0;
        virtual void useMicroUsb() = 0;

        virtual ~IMicroUsbPhone() = default;
    };

    class Android: public IMicroUsbPhone {
        bool connector { false };

        void useMicroUsb() override
        {
            connector = true;
            std::cout << "MicroUsb connected\n";
        }

        void recharge() override
        {
            if (connector)
            {
                std::cout << "Recharge started" << std::endl;
                std::cout << "Recharge finished" << std::endl;
            }
            else
            {
                std::cout << "Connect MicroUsb first" << std::endl;
            }
        }
    };


    class Iphone: public ILightningPhone
    {
        bool connector { false };

    public:
        void useLightning() override
        {
            connector = true;
            std::cout << "Lightning connected\n";
        }

        void recharge() override
        {
            if (connector)
            {
                std::cout << "Recharge started" << std::endl;
                std::cout << "Recharge finished" << std::endl;
            }
            else
            {
                std::cout << "Connect Lightning first" << std::endl;
            }
        }
    };

    class LightningToMicroUsbAdapter: public IMicroUsbPhone
    {
        std::shared_ptr<ILightningPhone> lightningPhone;

    public:
        explicit LightningToMicroUsbAdapter(std::shared_ptr<ILightningPhone> phone):
                lightningPhone {std::move( phone )} {
        }

        void useMicroUsb() override {
            std::cout << "MicroUsb connected" << std::endl;
            lightningPhone->useLightning();
        }

        void recharge() override {
            lightningPhone->recharge();
        }
    };
}

void PhoneChargeAdapterTest()
{
    using namespace Adapter;

    auto rechargeMicroUsbPhone = [](IMicroUsbPhone* phone) {
        phone->useMicroUsb();
        phone->recharge();
    };

    auto rechargeLightningPhone = [](ILightningPhone* phone) {
        phone->useLightning();
        phone->recharge();
    };

    std::shared_ptr<Android> android { std::make_shared<Android>()};
    std::shared_ptr<Iphone> iPhone { std::make_shared<Iphone>()};

    std::cout << "----------------------------- Recharging android with MicroUsb --------------------------------" << std::endl;

    rechargeMicroUsbPhone(android.get());

    std::cout << "------------------------------ Recharging iPhone with Lightning -------------------------------" << std::endl;

    rechargeLightningPhone(iPhone.get());

    std::cout << "------------------------------ Recharging iPhone with MicroUsb ---------------------------------" << std::endl;

    std::shared_ptr<IMicroUsbPhone> usbAdapter { std::make_shared<LightningToMicroUsbAdapter>(iPhone)};
    rechargeMicroUsbPhone(usbAdapter.get());
}
