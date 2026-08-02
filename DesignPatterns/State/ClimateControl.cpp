/**============================================================================
Name        : ClimateControl.cpp
Created on  : 02.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ClimateControl.cpp
============================================================================**/

#include "state.hpp"

#include <iostream>
#include <string_view>
#include <memory>
#include <utility>
#include <optional>
#include <variant>

namespace
{
    struct IState
    {
        [[nodiscard]]
        virtual std::string GetName() const noexcept = 0;

        virtual void Freeze(std::shared_ptr<class StateContext> context) = 0;
        virtual void Heat(std::shared_ptr<class StateContext> context) = 0;

        virtual ~IState() = default;
    };

    struct State : IState
    {
        std::string name;

        explicit State(std::string name): name(std::move(name)) {
        }

        [[nodiscard]]
        std::string GetName() const noexcept override  {
            return this->name;
        }
    };

    class StateContext : public std::enable_shared_from_this<StateContext>
    {
    public:
        explicit StateContext(std::unique_ptr<IState> state);
        virtual ~StateContext();

    public:
        void Freeze();
        void Heat();
        void SetState(std::unique_ptr<IState> state) noexcept;
        std::unique_ptr<IState> GetState() noexcept;

    private:
        std::unique_ptr<IState> state;
    };


    class SolidState : public State {
    public:
        SolidState();
        void Freeze(std::shared_ptr<StateContext> context) override;
        void Heat(std::shared_ptr<StateContext> context) override;
    };


    class LiquidState : public State {
    public:
        LiquidState();
        void Freeze(std::shared_ptr<StateContext> context) override;
        void Heat(std::shared_ptr<StateContext> context) override;
    };


    class GasState : public State {
    public:
        GasState();
        void Freeze(std::shared_ptr<StateContext> context) override;
        void Heat(std::shared_ptr<StateContext> context) override;
    };


    StateContext::StateContext(std::unique_ptr<IState> state):
            state(state.release()) {
    }

    void StateContext::Freeze() {
        std::cout << "Freezing " << state->GetName() << "..." << std::endl;
        this->state->Freeze(this->shared_from_this());
    }

    void StateContext::Heat() {
        std::cout << "Heating " << state->GetName() << "..." << std::endl;
        this->state->Heat(this->shared_from_this());
    }

    void StateContext::SetState(std::unique_ptr<IState> st) noexcept {
        std::cout << "Changing state from " << this->state->GetName() << " to " << st->GetName() << "..." << std::endl;
        this->state = std::move(st);
    }

    std::unique_ptr<IState> StateContext::GetState() noexcept {
        return std::move(this->state);
    }

    StateContext::~StateContext() {
        std::cout << "Destroying stata " << this->state->GetName() << std::endl;
    }


    /** SolidState class: **/
    SolidState::SolidState() : State("Solid") {
    }

    void SolidState::Freeze([[maybe_unused]] std::shared_ptr<StateContext> context) {
        std::cout << "Nothing happens" << std::endl;
    }

    void SolidState::Heat(std::shared_ptr<StateContext> context) {
        context->SetState(std::make_unique<LiquidState>());
    }


    /** LiquidState class: **/
    LiquidState::LiquidState(): State("Liquid") {
    }

    void LiquidState::Freeze(std::shared_ptr<StateContext> context) {
        context->SetState(std::make_unique<SolidState>());
    }

    void LiquidState::Heat(std::shared_ptr<StateContext> context) {
        context->SetState(std::make_unique<GasState>());
    }


    /** GasState class: **/
    GasState::GasState() : State("Gas")  {
    }

    void GasState::Freeze(std::shared_ptr<StateContext> context) {
        context->SetState(std::make_unique<LiquidState>());
    }

    void GasState::Heat([[maybe_unused]] std::shared_ptr<StateContext> context) {
        std::cout << "Nothing happens" << std::endl;
    }
}

void state::climate_control::TestAll()
{
    std::shared_ptr<StateContext> sc = std::make_shared<StateContext>(std::make_unique<SolidState>());
    sc->Heat();
    sc->Heat();
    sc->Heat();
    sc->Freeze();
    sc->Freeze();
    sc->Freeze();
}

/**
Heating Solid...
Changing state from Solid to Liquid...
Heating Liquid...
Changing state from Liquid to Gas...
Heating Gas...
Nothing happens
Freezing Gas...
Changing state from Gas to Liquid...
Freezing Liquid...
Changing state from Liquid to Solid...
Freezing Solid...
Nothing happens
Destroying stata Solid
**/