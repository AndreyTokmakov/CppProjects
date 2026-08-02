/**============================================================================
Name        : StaticTransitionTable.cpp
Created on  : 02.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticTransitionTable.cpp
============================================================================**/

#include "state.hpp"

#include <iostream>
#include <tuple>
#include <variant>
#include <type_traits>

/**
    Compile-Time Finite State Machine (FSM)

    Данный пример демонстрирует один из вариантов реализации конечного автомата (Finite State Machine, FSM),
    в котором описание переходов полностью выполняется на этапе компиляции.

    Основная идея заключается в том, что таблица переходов представляется не в виде большого switch/case,
    цепочек if/else  или виртуальных методов, а в виде набора типов Transition<FromState, Event, ToState>.
    Во время обработки события движок автомата определяет следующий тип состояния, используя исключительно шаблонную метапрограмму.

    ----------------------------------------------------------------------------
    Логика работы
    ----------------------------------------------------------------------------

    1. Автомат хранит текущее состояние в std::variant.

       using State = std::variant<
           Idle,
           Connecting,
           Connected>;

       В каждый момент времени активным является только один тип состояния.

    2. Каждое событие представляется отдельным типом.

       struct EventConnect {};
       struct EventConnected {};
       struct EventDisconnect {};

       Это обеспечивает полную типобезопасность и отсутствие необходимости использовать enum.

    3. Все переходы описываются декларативно.

       using TransitionTable = std::tuple<
           Transition<Idle, EventConnect, Connecting>,
           Transition<Connecting, EventConnected, Connected>,
           Transition<Connected, EventDisconnect, Idle>>;

       Каждая запись означает:

           FromState + Event -> ToState

       То есть таблица переходов является обычным списком типов.

    4. При вызове dispatch(event):

       • std::visit определяет текущий тип состояния.
       • Тип события известен из шаблонного параметра dispatch().
       • По паре (CurrentState, Event) во время компиляции ищется соответствующая запись
         Transition<CurrentState, Event, NewState>.
       • Если переход найден, создаётся новое состояние NewState.
       • Если переход отсутствует, автомат остаётся в текущем состоянии.

    5. Поиск перехода полностью выполняется на этапе компиляции.

       В рантайме отсутствуют:

       • switch
       • if/else по типам состояний
       • виртуальные функции
       • RTTI для выбора перехода

       Во время выполнения остаётся только вызов std::visit и создание нового объекта состояния.

    ----------------------------------------------------------------------------
    Детали реализации
    ----------------------------------------------------------------------------

    Transition

        Описывает одно правило перехода. содержит исключительно информацию о типах и не содержит логики.

            Transition<Idle, EventConnect, Connecting>

    TransitionTable

        Представляет собой compile-time таблицу переходов.
        По сути это аналог UML State Diagram, записанный средствами шаблонов C++.

    FindTransition

        Рекурсивный шаблон, который просматривает TransitionTable и ищет запись, соответствующую
        текущему состоянию и типу события.

    NextState

        Возвращает тип следующего состояния. Если переход отсутствует, возвращается тип void.

            using Next = NextState<CurrentState, Event, TransitionTable>::Type;

    dispatch()

        Выполняет обработку события. dispatch() не знает ничего о конкретных состояниях и событиях.
        Его задача состоит только в том, чтобы:

        • определить активное состояние;
        • вычислить следующий тип состояния;
        • заменить текущее состояние новым экземпляром.

        Благодаря этому StateMachine остаётся полностью универсальным.

    ----------------------------------------------------------------------------
    Преимущества подхода
    ----------------------------------------------------------------------------

    • отсутствуют switch/case по состояниям;
    • отсутствуют виртуальные функции;
    • отсутствует наследование;
    • высокая типобезопасность;
    • таблица переходов проверяется компилятором;
    • все состояния являются обычными структурами;
    • переходы описываются декларативно;
    • движок автомата не зависит от предметной области.

    ----------------------------------------------------------------------------
    Ограничения
    ----------------------------------------------------------------------------

    Данный подход хорошо подходит только для автоматов, в которых переход определяется
    исключительно текущим состоянием и типом события.

    Если переход требует:

    • изменения внутренних данных состояния;
    • выполнения побочных действий;
    • проверки guard-условий;
    • сложной логики построения нового состояния,

    то одной записи

        Transition<From, Event, To>

    становится недостаточно.

    В этом случае обычно приходится вводить дополнительные сущности (Action, Guard и т.п.),
    что значительно усложняет реализацию.

    Именно поэтому данный подход чаще используется как демонстрация возможностей шаблонной
    метапрограммы или в специализированных библиотеках (например, Boost.SML), тогда как для
    небольших и средних автоматов более практичным зачастую оказывается Visitor-подход с
    перегруженными operator(), где описание перехода и его логика находятся в одном месте.

    ----------------------------------------------------------------------------
    Когда применять
    ----------------------------------------------------------------------------

    Подход рекомендуется использовать, если:

    • переход полностью определяется типами состояния и события;
    • отсутствует сложная бизнес-логика переходов;
    • требуется максимально универсальный движок автомата;
    • количество автоматов велико и желательно описывать их декларативно;
    • необходимо анализировать таблицу переходов во время компиляции.

    Подход не рекомендуется использовать, если:

    • переходы содержат значительную бизнес-логику;
    • состояние имеет сложные внутренние данные;
    • переходы требуют изменения существующего состояния;
    • необходимо выполнять большое количество действий при смене состояния.

**/

namespace
{
    struct Idle { };
    struct Connecting { };
    struct Connected { };
    struct EventConnect { };
    struct EventConnected { };
    struct EventDisconnect {};

    template<typename FromStateT, typename ToStateT, typename EventT>
    struct Transition
    {
        using FromState = FromStateT;
        using ToState = ToStateT;
        using Event = EventT;
    };

    using TransitionTable = std::tuple<Transition<Idle, Connecting, EventConnect>,
                                       Transition<Connecting, Connected, EventConnected>,
                                       Transition<Connected, Idle,  EventDisconnect>>;

    template<typename StateType, typename EventT, typename... Transitions>
    struct FindTransition;

    template<typename StateType, typename EventType>
    struct FindTransition<StateType, EventType>
    {
        using Type = void;
    };

    template<typename StateType, typename EventType, typename Head, typename... Tail>
    struct FindTransition<StateType, EventType, Head, Tail...>
    {
        using Type = std::conditional_t<
            std::is_same_v<StateType, typename Head::FromState> &&
            std::is_same_v<EventType, typename Head::Event>,
            typename Head::ToState,
            typename FindTransition<StateType, EventType, Tail...>::Type>;
    };

    template<typename StateType, typename EventType, typename TableT>
    struct NextState;

    template<typename StateType, typename EventType, typename... Transitions>
    struct NextState<StateType, EventType, std::tuple<Transitions...>>
    {
        using Type = typename FindTransition<StateType, EventType, Transitions...>::Type;
    };

    using State = std::variant<Idle, Connecting, Connected>;

    struct StateMachine
    {
        State currentState { Idle{} };

        template<typename EventType>
        void dispatch(const EventType& event)
        {
            std::visit([&]<typename Ty>(Ty&) {
                using CurrentState = std::decay_t<Ty>;
                using NewState = NextState<CurrentState, EventType, TransitionTable>::Type;
                if constexpr (!std::is_void_v<NewState>)
                {
                    std::cout << typeid(CurrentState).name() << " -> "<< typeid(NewState).name() << '\n';
                    this->currentState = NewState{};
                }
                else {
                    std::cerr << "Unknown transition\n";
                }
            }, currentState);}
    };
}


void state::static_transition_table::TestAll()
{
    StateMachine machine;

    machine.dispatch(EventConnect{});
    machine.dispatch(EventConnected{});
    machine.dispatch(EventDisconnect{});
}

/**
N12_GLOBAL__N_14IdleE -> N12_GLOBAL__N_110ConnectingE
N12_GLOBAL__N_110ConnectingE -> N12_GLOBAL__N_19ConnectedE
N12_GLOBAL__N_19ConnectedE -> N12_GLOBAL__N_14IdleE
**/