/**============================================================================
Name        : Command.h
Created on  : 12.04.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Command pattern src
============================================================================**/

#include "Command.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace
{
    // Command Interface
    struct ICommand {
        virtual void execute() = 0;
        virtual ~ICommand() = default;
    };

    // Receiver Class
    struct Light final
    {
        virtual void on() {
            std::cout << "Light: The light is ON" << std::endl;
        }

        virtual void off() {
            std::cout << "Light: The light is OFF" << std::endl;
        }

        virtual ~Light() = default;
    };

    struct LightControlCommand: ICommand
    {
        explicit LightControlCommand(std::shared_ptr<Light> light) : mLight(std::move(light)) {
        }

        void execute() override = 0;

        std::shared_ptr<Light> mLight;
    };

    // Command for turning on the light
    struct LightOnCommand final : LightControlCommand
    {
        using LightControlCommand::LightControlCommand;

        void execute() override {
            std::cout << "LightOnCommand::execute()\n";
            this->mLight->on();
        }
    };

    // Command for turning off the light
    struct LightOffCommand: LightControlCommand
    {
        using LightControlCommand::LightControlCommand;

        void execute() override {
            std::cout << "LightOffCommand::execute()\n";
            this->mLight->off();
        }
    };

    // Invoker: Stores the ConcreteCommand object
    struct RemoteControl
    {
        std::shared_ptr<ICommand> command;

        void setCommand(const std::shared_ptr<ICommand>& cmd) {
            this->command = cmd;
        }

        void buttonPressed() {
            command->execute();
        }
    };

    void Test()
    {
        std::shared_ptr<Light> light = std::make_shared<Light>();
        const std::shared_ptr<RemoteControl> control = std::make_shared<RemoteControl>();

        // concrete Command objects
        // std::unique_ptr<LightOnCommand> lightOn = std::make_unique<LightOnCommand>(new LightOnCommand(light));
        std::shared_ptr<LightOnCommand> lightOn = std::make_shared<LightOnCommand>(light);
        std::shared_ptr<LightOffCommand> lightOff = std::make_shared<LightOffCommand>(light);

        // execute
        control->setCommand(lightOn);
        control->buttonPressed();
        control->setCommand(lightOff);
        control->buttonPressed();
    }
}


void command::testAll()
{
    // method_ptr::TestAll();
    // filesystem_command_dispatcher::TestAll();
    // Switch_ON_OFF_Light::Test();
    // command_crtp::TestAll();
    command_bus_handler::TestAll();
    command_bus_handler_crtp::TestAll();
}
