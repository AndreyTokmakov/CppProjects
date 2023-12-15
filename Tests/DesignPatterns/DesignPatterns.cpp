/**============================================================================
Name        : DesignPatterns.h
Created on  : 27.08.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : DesignPatterns
============================================================================**/

#include "DesignPatterns.h"

#include <deque>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <variant>
#include <optional>
#include <sstream>
#include <future>
#include <thread>
#include <initializer_list>

namespace DesignPatterns::Singleton
{
    class FileRepository
    {
    private:
        std::deque<std::string> files;

        FileRepository() {
            std::cerr << " [LOG] File Repository Initialized." << "\n";
        }

    public:

        FileRepository(const FileRepository&) = delete;
        FileRepository& operator=(const FileRepository&) = delete;

        ~FileRepository(){
            std::cerr << " [LOG] File Repository Deleted. Ok." << "\n";
        }

        // Return a reference to not allow client code to delete object.
        static FileRepository& getInstance() {
            static auto instance = std::unique_ptr<FileRepository>{nullptr};
            if(!instance) /** Initialized once - lazy initialization **/
                instance.reset(new FileRepository);
            return *instance;
        }

        void addFile(std::string filename) noexcept{
            files.push_back(std::move(filename));
        }

        void clearFiles() noexcept {
            files.clear();
        }

        void showFiles() const noexcept {
            for (const auto& file: files)
                std::cout << " File = " << file << std::endl;
        }
    };

    // ----------------------------------

    void Test()
    {
        FileRepository& repo1 = FileRepository::getInstance();
        repo1.addFile("CashFlowStatement.txt");
        repo1.addFile("Balance-Sheet.dat");
        repo1.addFile("Sales-Report.csv");

        FileRepository& repo2 = FileRepository::getInstance();

        std::cout << std::boolalpha << "Same object? (&repo == &repo1 ?) = " << (&repo1 == &repo2) << std::endl;

        std::cout << "\nRepository files" << std::endl;
        repo2.showFiles();

        std::cout << "\nAdd more files" << std::endl;
        repo2.addFile("fileX1.pdf");
        repo2.addFile("fileX2.pdf");
        repo2.addFile("fileX3.pdf");
        repo2.showFiles();
    }
};

namespace DesignPatterns::Strategy
{
    // Strategy interface
    struct IStrategy
    {
        virtual ~IStrategy() = default;

        // Essential: Algorithm encapsulated by strategy object
        [[nodiscard]]
        virtual double compute(double x, double y) const = 0;

        // Optional: Provides strategy metadata
        [[nodiscard]]
        virtual std::string name() const noexcept = 0;

        // Clone this object (Note: This is a virtual constructor)
        [[nodiscard]]
        virtual IStrategy* clone() const = 0;
    };


    class AddStrategy: public IStrategy {
    public:
        [[nodiscard]]
        std::string name() const noexcept override {
            return "add";
        }

        [[nodiscard]]
        double compute(double x, double y) const override  {
            return x + y;
        }

        [[nodiscard]]
        IStrategy* clone() const override {
            std::cerr << " [TRACE] AddStrategy => I was cloned" << "\n";
            return new AddStrategy(*this);
        }
    };


    struct MultStrategy: public IStrategy {
    public:
        [[nodiscard]]
        std::string name() const noexcept override {
            return "mult";
        }

        [[nodiscard]]
        double compute(double x, double y) const override  {
            return x * y;
        }

        [[nodiscard]]
        IStrategy* clone() const override {
            std::cerr << " [TRACE] MultStrategy => I was cloned" << "\n";
            return new MultStrategy(*this);
        }
    };

    struct LinearCombStrategy: public IStrategy {
        double a { 0.0 }, b { 0.0 }, c { 0.0 };

        LinearCombStrategy(double a, double b, double c): a(a), b(b), c(c) {
        }

        [[nodiscard]]
        std::string name() const noexcept override {
            return "Linear combination a * x + b * y + c";
        }

        [[nodiscard]]
        double compute(double x, double y) const override  {
            return a * x + b * y + c;
        }

        [[nodiscard]]
        IStrategy* clone() const override {
            std::cerr << " [TRACE] LinearCombStrategy => I was cloned" << "\n";
            return new LinearCombStrategy(*this);
        }
    };



    class Context {
    private:
        std::unique_ptr<IStrategy> strategy;

    public:
        Context(): strategy {nullptr} { }
        explicit Context(IStrategy* s): strategy {s} { }
        explicit Context(const IStrategy& s): strategy {s.clone()} { }

        void setStrategy(IStrategy* s) {
            strategy.reset(s);
        }

        auto setStrategy(const IStrategy& s){
            strategy.reset(s.clone());
        }

        void compute(double x, double y) {
            if (nullptr == strategy)
                throw std::runtime_error("Error: strategy not set");

            const double result = strategy->compute(x, y);
            std::cout << " strategy = " << strategy->name() << " "
                      << "( x = " << x << " ; "
                      << "y = " << y << " )"
                      << "\n" ;
            std::cout << "Result = " << result << "\n";
        }
    };

    void Test()
    {
        Context ctx;
        std::cout << "==== Strategy = ADD ====" << "\n";
        ctx.setStrategy(new AddStrategy);
        ctx.compute(3.0, 4.0);

        std::cout << "==== Strategy = MULT ====" << "\n";
        ctx.setStrategy(new MultStrategy);
        ctx.compute(3.0, 4.0);

        std::cout << "==== Strategy = Linear combination ====" << "\n";
        ctx.setStrategy(new LinearCombStrategy(5, 3, 4));
        ctx.compute(3.0, 4.0);

        std::cout << "==== Strategy = Linear combination [2] ====" << "\n";
        auto comb1 = LinearCombStrategy(6.0, 5.0, 10.0);
        // Copy stack-allocated object comb1 using the virtual constructor

        ctx.setStrategy(comb1);
        ctx.compute(5.0, 3.0);

        std::cout << "==== Strategy = Linear combination [2] ====" << "\n";
        // Copy stack-allocated temporary object comb1 using the virtual constructor clone

        ctx.setStrategy(LinearCombStrategy{6.0, 5.0, 10.0});
        ctx.compute(2.0, 6.0);
    }
}

namespace DesignPatterns::Strategy_Text
{
    enum class Format {
        Markdown,
        Html
    };

    struct ListStrategy
    {
        virtual void add_list_item(std::ostringstream&, const std::string&) { };
        virtual void start(std::ostringstream&) { };
        virtual void end(std::ostringstream&) { };

        virtual ~ListStrategy() = default;
    };

    struct MarkdownListStrategy: ListStrategy {
        void add_list_item(std::ostringstream& oss, const std::string& item) override {
            oss << " - " << item << std::endl;
        }
    };

    struct HtmlListStrategy: ListStrategy
    {
        void start(std::ostringstream& oss) override {
            oss << "<ul>" << std::endl;
        }

        void end(std::ostringstream& oss) override {
            oss << "</ul>" << std::endl;
        }

        void add_list_item(std::ostringstream& oss, const std::string& item) override {
            oss << "\t<li>" << item << "</li>" << std::endl;
        }
    };

    struct TextProcessor
    {
        void clear()
        {
            strStream.str("");
            strStream.clear();
        }

        void append_list(const std::vector<std::string>& items) {
            listStrategy->start(strStream);
            for (auto& item: items)
                listStrategy->add_list_item(strStream, item);
            listStrategy->end(strStream);
        }

        void set_output_format(Format format)
        {
            switch (format) {
                case Format::Markdown: listStrategy = std::make_unique<MarkdownListStrategy>(); break;
                case Format::Html: listStrategy = std::make_unique<HtmlListStrategy>(); break;
            }
        }

        std::string str() {
            return strStream.str();
        }

    private:
        std::ostringstream             strStream;
        std::unique_ptr<ListStrategy>  listStrategy;
    };


    void Test() {
        // markdown
        TextProcessor tp;
        tp.set_output_format(Format::Markdown);
        tp.append_list({ "foo", "bar", "baz" });

        std::cout << tp.str() << std::endl;
        // html
        tp.clear();
        tp.set_output_format(Format::Html);
        tp.append_list({ "foo", "bar", "baz" });
        std::cout << tp.str() << std::endl;
    }
}

// Simple Virtual Copy Constructor
namespace DesignPatterns::CRTP
{
    class IBase
    {
    public:
        // Destructor always virtual to avoid memory leak,
        virtual ~IBase() = default;

        // "virtual copy constructor"
        [[nodiscard]]
        virtual std::unique_ptr<IBase> clone() const = 0;

        // "virtual default constructor"
        [[nodiscard]]
        virtual std::unique_ptr<IBase> create() const = 0;

        [[nodiscard]]
        virtual std::string getID() const = 0;

        // virtual void setID(std::string id) = 0;  // INFO: No need to demo

        virtual void show() const = 0;
    };

    template<typename Base, typename Derived>
    class Copyable : public Base {
    private:
        [[nodiscard]]
        inline const Derived &self() const noexcept {
            return *static_cast<const Derived *const>(this);
        }

    public:
        virtual ~Copyable() = default;

        [[nodiscard]]
        std::unique_ptr<Base> clone() const override {
            std::cout << "Cloning " << typeid(Derived).name() << std::endl;
            return std::make_unique<Derived>(this->self());
        }

        [[nodiscard]]
        std::unique_ptr<Base> create() const override {
            return std::make_unique<Derived>();
        }
    };

    class DerivedA : public Copyable<IBase, DerivedA> {
    private:
        std::string id;
    public:
        explicit DerivedA() : id("unnamed-A") {}

        explicit DerivedA(std::string id) : id { std::move(id) } {}

        [[nodiscard]]
        std::string getID() const override { return id; }
        // void setID(std::string id) override { this->id = id; } // INFO: No need to demo

        void show() const override {
            std::cout << " => Class DerivedA - id = " << id << "\n";
        }
    };


    class DerivedB : public Copyable<IBase, DerivedB> {
    private:
        std::string id;
    public:
        DerivedB() : id("unnamed-B") {}

        explicit DerivedB(std::string id) : id { std::move(id) } {}

        [[nodiscard]]
        std::string getID() const override { return id; }
        // void setID(std::string id) override { this->id = id; } // INFO: No need to demo

        void show() const override {
            std::cout << " => Class DerivedB - id = " << id << "\n";
        }
    };

    void Test()
    {
        DerivedA a("objectA");
        DerivedB b("objectB");

        a.show();
        b.show();

        std::cout << "----------------------------------------------------------\n";

        IBase *aPtr = &a, *bPtr = &b;
        aPtr->show();
        bPtr->show();

        std::cout << "----------------------------------------------------------\n";

        std::unique_ptr<IBase> clone = aPtr->clone();
        clone->show();

        clone = bPtr->clone();
        clone->show();
    }
}

namespace DesignPatterns::Chain_of_Responsibility
{
    class Handler {
    public:
        virtual std::shared_ptr<Handler> SetNext(const std::shared_ptr<Handler>& handler) = 0;

        [[nodiscard]]
        virtual std::string Handle(std::string request) = 0;

        virtual ~Handler() = default;
    };

    class AbstractHandler : public Handler
    {
        std::shared_ptr<Handler> next_handler { nullptr };

    public:
        AbstractHandler() : next_handler(nullptr) {
        }

        std::shared_ptr<Handler> SetNext(const std::shared_ptr<Handler>& handler) override {
            this->next_handler = handler;
            return handler;
        }

        [[nodiscard]]
        std::string Handle(std::string request) override {
            return this->next_handler ? this->next_handler->Handle(request) : std::string {};
        }
    };

    class MonkeyHandler : public AbstractHandler {
    public:
        std::string Handle(std::string request) override
        {
            std::cout << "Monkey: ";
            if (request == "Banana")
                return std::string {"I'll eat the "}.append(request).append(".\n");

            std::cout << "I will not...\n";
            return AbstractHandler::Handle(request);
        }
    };

    class SquirrelHandler : public AbstractHandler {
    public:
        std::string Handle(std::string request) override
        {
            std::cout << "Squirrel: ";
            if (request == "Nut")
                return std::string {"I'll eat the "}.append(request).append(".\n");

            std::cout << "I will not...\n";
            return AbstractHandler::Handle(request);
        }
    };

    struct DogHandler : public AbstractHandler {
    public:
        std::string Handle(std::string request) override
        {
            std::cout << "Dog: ";
            if (request == "MeatBall")
                return std::string {"I'll eat the "}.append(request).append(".\n");

            std::cout << "I will not...\n";
            return AbstractHandler::Handle(request);
        }
    };


    void ClientCode(const std::shared_ptr<Handler>& handler)
    {
        for (std::string f : {"Nut", "Banana", "Cup of coffee"})
        {
            std::cout << "\nClient: Who wants a " << f << "?\n";
            const std::string result = handler->Handle(f);
            if (!result.empty()) {
                std::cout  << result;
            } else {
                std::cout << "  " << f << " was left untouched.\n";
            }
        }
    }

    void Test()
    {
        auto monkey  { std::make_shared<MonkeyHandler>() };
        auto squirrel  { std::make_shared<SquirrelHandler>() };
        auto dog  { std::make_shared<DogHandler>() };

        monkey->SetNext(squirrel)->SetNext(dog);

        std::cout << "* * * * * Chain: Monkey ==> Squirrel ==> Dog * * * * * \n\n";
        ClientCode(monkey);


        // std::cout << "\nSubchain: Squirrel > Dog\n\n";
        // ClientCode(squirrel);
    }
}

namespace DesignPatterns::HouseFacade
{
    struct Alarm
    {
        void alarmOn() {
            std::cout << "Alarm is on and house is secured"<<std::endl;
        }

        void alarmOff() {
            std::cout << "Alarm is off and you can go into the house"<<std::endl;
        }
    };

    struct Ac
    {
        void acOn() {
            std::cout << "Ac is on"<<std::endl;
        }

        void acOff() {
            std::cout << "AC is off"<<std::endl;
        }
    };

    struct Tv
    {
        void tvOn() {
            std::cout << "Tv is on"<<std::endl;
        }

        void tvOff() {
            std::cout << "TV is off"<<std::endl;
        }
    };

    struct HouseFacade
    {
        Alarm alarm;
        Ac ac;
        Tv tv;

        void goToWork() {
            ac.acOff();
            tv.tvOff();
            alarm.alarmOn();
        }

        void comeHome() {
            alarm.alarmOff();
            ac.acOn();
            tv.tvOn();
        }
    };

    void Test()
    {
        HouseFacade hf;

        hf.goToWork();
        hf.comeHome();
    }
};

namespace DesignPatterns::Mediator
{
    struct ChatRoom {
        virtual void broadcast(std::string from, std::string msg) = 0;
        virtual void message(std::string from, std::string to, std::string msg) = 0;

        virtual ~ChatRoom() = default;
    };

    struct Person {
        std::string name;
        ChatRoom*   m_room {nullptr};
        // std::vector<std::string>  chatLog;

        explicit Person(std::string n) : name(std::move(n)) {
        }

        void say(std::string msg) const {
            m_room->broadcast(name, std::move(msg));
        }
        void pm(std::string to, std::string msg) const {
            m_room->message(name, std::move(to), std::move(msg));
        }

        void receive(const std::string& from, const std::string& msg) const
        {
            std::string s{from + ": \"" + msg + "\""};
            std::cout << "[" << name << "'s chat session]" << s << "\n";
            // chatLog.emplace_back(s);
        }
    };

    struct GoogleChat : ChatRoom
    {
        std::vector<Person*> people;

        void broadcast(std::string from, std::string msg) override {
            for (auto p : people)
                if (p->name != from)
                    p->receive(from, msg);
        }

        void join(Person *p)
        {
            std::string join_msg = p->name + " joins the chat";
            broadcast("room", join_msg);
            p->m_room = this;
            people.push_back(p);
        }

        void message(std::string from, std::string to, std::string msg) override
        {
            auto target = find_if(begin(people), end(people),
                    [&](const Person *p) { return p->name == to;
            });
            if (target != end(people))
                (*target)->receive(from, msg);
        }
    };

    void Test()
    {
        GoogleChat room;
        Person john{"John"}, jane{"Jane"}, simon{"Simon"};

        room.join(&john);
        room.join(&jane);

        john.say("hi room");
        jane.say("oh, hey john");

        room.join(&simon);
        simon.say("hi everyone!");
        jane.pm("Simon", "glad you found us, simon!");

    }
}

namespace DesignPatterns::State
{
    enum class Event {
        connect,
        connected,
        disconnect,
        timeout
    };

    inline std::ostream &operator<<(std::ostream &os, const Event event) {
        switch (event) {
            case Event::connect: os << "connect"; break;
            case Event::connected: os << "connected"; break;
            case Event::disconnect: os << "disconnect"; break;
            case Event::timeout: os << "timeout"; break;
        }
        return os;
    }

    struct IState {
        virtual std::unique_ptr<IState> onEvent(Event event) = 0;

        [[nodiscard]]
        virtual std::string getName() const  = 0;

        virtual ~IState() = default;
    };

    /** States: **/
    struct Idle : IState {
        std::unique_ptr<IState> onEvent(Event event) override;

        [[nodiscard]]
        std::string getName() const override;
    };

    struct Connecting : IState {
        std::unique_ptr<IState> onEvent(Event event) override;

        [[nodiscard]]
        std::string getName() const override;

    private:
        uint32_t                    m_trial = 0;
        static constexpr uint8_t    m_max_trial = 3;
    };

    struct Connected : IState {
        std::unique_ptr<IState> onEvent(Event e) override;

        [[nodiscard]]
        std::string getName() const override;
    };




    std::unique_ptr<IState> Idle::onEvent(Event event) {
        std::cout << getName() << " ==> " << event;
        if (Event::connect == event)
            return std::make_unique<Connecting>();
        return nullptr;
    }

    std::string Idle::getName() const {
        return {"Idle"};
    }

    std::unique_ptr<IState> Connecting::onEvent(Event event)
    {
        std::cout << getName() << " ==> " << event;
        switch (event) {
            case Event::connected:
                return std::make_unique<Connected>();
                break;
            case Event::timeout:
                return ++m_trial < m_max_trial ? nullptr : std::make_unique<Idle>();
                break;
            case Event::connect:
                [[fallthrough]];
            case Event::disconnect:
                return nullptr;
                break;
        }
        return nullptr;
    }

    std::string Connecting::getName() const {
        return {"Connecting"};
    }

    std::unique_ptr<IState> Connected::onEvent(Event event)
    {
        std::cout << getName() << " ==> " << event;
        if (Event::disconnect == event)
            return std::make_unique<Idle>();
        return nullptr;
    }

    std::string Connected::getName() const {
        return {"Connected"};
    }

    struct Bluetooth
    {
        std::unique_ptr<IState> state = std::make_unique<Idle>();

        void dispatch(Event e)
        {
            auto stateNew = state->onEvent(e);
            if (stateNew)
                state = std::move(stateNew);

            std::cout << " ==> " << state->getName() << std::endl;
        }

        template <typename... Events>
        void establish_connection(Events... e) {
            (dispatch(e), ...);
        }
    };

    void Test()
    {
        Bluetooth bl;
        bl.establish_connection(Event::connect, Event::timeout, Event::connected, Event::disconnect);
    }
};

namespace DesignPatterns::State_Visitor
{
    struct EventConnect { std::string m_address; };
    struct EventConnected { };
    struct EventDisconnect { };
    struct EventTimeout { };

    using Event = std::variant<EventConnect,EventConnected, EventDisconnect, EventTimeout>;

    struct Idle { };

    struct Connecting {
        std::string                 m_address;
        uint32_t                    m_trial = 0;
        static constexpr uint8_t    m_max_trial = 3;
    };
    struct Connected { };

    using State = std::variant<Idle, Connecting, Connected>;


    struct TransitionsHandler
    {
        std::optional<State> operator()(Idle&, const EventConnect &e) {
            std::cout << "Idle -> Connect" << std::endl;
            return Connecting{e.m_address};
        }

        std::optional<State> operator()(Connecting&, const EventConnected &) {
            std::cout << "Connecting -> Connected" << std::endl;
            return Connected{};
        }

        std::optional<State> operator()(Connecting &s, const EventTimeout &) {
            std::cout << "Connecting -> Timeout" << std::endl;
            return ++s.m_trial < Connecting::m_max_trial ? std::nullopt : std::optional<State>(Idle{});
        }

        std::optional<State> operator()(Connected&, const EventDisconnect &) {
            std::cout << "Connected -> Disconnect" << std::endl;
            return Idle{};
        }

        template <typename State_t, typename Event_t>
        std::optional<State> operator()(State_t &, const Event_t &) const {
            std::cout << "Unknown" << std::endl;
            return std::nullopt;
        }
    };

    template <typename StateVariant, typename EventVariant, typename Transitions>
    struct Bluetooth {
        StateVariant currentState;

        void dispatch(const EventVariant &event)
        {
            std::optional<StateVariant> new_state = visit(Transitions{}, currentState, event);
            if (new_state)
                currentState = *std::move(new_state);
        }

        template <typename... Events>
        void establish_connection(Events... e) {
            (dispatch(e), ...);
        }
    };

    void Test()
    {
        Bluetooth<State, Event, TransitionsHandler> bl;
        bl.establish_connection(EventConnect{"AA:BB:CC:DD"},
                                EventTimeout{},
                                EventConnected{},
                                EventDisconnect{});
    }
}

namespace DesignPatterns::Builder
{

    class User
    {
    private:
        uint32_t     id = 0;
        std::string  name;
        std::string  lastName;
        std::string  email;

    public:
        User() {
            std::cout << "User::User()" << std::endl;
        }

        ~User() {
            std::cout << "User::~User()" << std::endl;
        }

        User(const User& u): id { u.id }, name { u.name },
                             lastName { u.lastName }, email { u.email }
        {
            std::cout << "User::User(const User&)" << std::endl;
        }

        User& operator=(const User&)
        {
            std::cout << "User operator=(const User& u)" << std::endl;
            //
            return *this;
        }

        User(User&& u) noexcept : id { std::exchange(u.id, 0) },
                                  name { std::exchange(u.name, "") },
                                  lastName { std::exchange(u.lastName, "") },
                                  email { std::exchange(u.email, "") }
        {
            std::cout << "User::User(User&&)" << std::endl;
        }

        User& operator=(User&&) noexcept
        {
            std::cout << "User operator=(User&& u) noexcept" << std::endl;
            //
            return *this;
        }

        void show() const noexcept
        {
            std::cout << "User {"
                      << "\n" << "  id        = " << id
                      << "\n" << "  name      = " << name
                      << "\n" << "  last name = " << lastName
                      << "\n" << "  email     = " << email
                      << "\n" << "}\n";
        }

        friend class UserBuilder;
    };

    class UserBuilder
    {
    private:
        User user {};

    public:

        UserBuilder& setID(ulong userID) {
            user.id = userID;
            return *this;
        }

        UserBuilder& setName(const std::string& name) {
            user.name = name;
            return *this;
        }

        UserBuilder& setLastName(const std::string& name){
            user.lastName = name;
            return *this;
        }

        UserBuilder& setEmail(const std::string& email){
            user.email = email;
            return *this;
        }

        User build() && {
            return std::move(this->user);
        }
    };


    User createUser() {
        return {};
    }

    void Test_User()
    {

        {
            User u = createUser();
        }
        std::cout << "-------------------------------------" << std::endl;

        {
            User u2 = UserBuilder().build();
        }

        /*
        User user0 = UserBuilder()
                .setID(2065).setName("John").setLastName("Von Neumman")
                .setEmail("nx098774a@sknmap.co").build();

        User user1 = UserBuilder()
                        .setID(1065).setName("Blaise")
                        .setLastName("Pascal").setEmail("dummyEmail@service1.co.uk").build();

        User user2 = UserBuilder()
                        .setID(2001).setName("Nikola")
                        .setLastName("Tesla").setEmail("wsx752@couk.com.sk").build();

        user0.show();
        user1.show();
        user2.show();
        */
    }
};

namespace DesignPatterns::Monostate
{
    /*
    struct MonoConfig
    {
        MonoConfig() {
            // ensure a single initialization outside of the static chain if we don't
            // need multi-threaded safety we can downgrade to a boolean flag
            std::call_once(onceFlag, initialize);
        }

        // Interface to access the monostate
        static uint32_t getValue() {
            return value;
        }

        static const std::string& getName() {
            return name;
        }

    private:

        static void initialize()
        {
            value = UINT32_C(42);
            name.assign("Hello World");
        }

        static inline std::once_flag onceFlag;
        static inline uint32_t value {0};
        static inline std::string name;
    };
    */

    // When combined with the PIMPL pattern we can mock/fake the global state:
    struct ImplIface {};

    struct Actual : ImplIface {
        static std::unique_ptr<ImplIface> make() {
            return std::make_unique<Actual>();
        }
    };

    struct Testing : ImplIface {
        static std::unique_ptr<ImplIface> make() {
            return std::make_unique<Testing>();
        }
    };

    // Switch active type based on testing/production
    using ActiveType = Testing;

    struct MonoPIMPL
    {
        MonoPIMPL() {
            std::call_once(flag, [] { impl = ActiveType::make(); });
        }
        /* expose ImplIface as any other PIMPL */
    private:
        static inline std::once_flag flag;
        static inline std::unique_ptr<ImplIface> impl;
    };


    void test()
    {
        MonoPIMPL x;
    }
}

namespace DesignPatterns::Observer
{
    template< typename Subject, typename StateTag >
    class Observer
    {
    public:
        using OnUpdate = void (*)(Subject const&,StateTag);

        // No virtual destructor necessary

        explicit Observer(OnUpdate onUpdate) : onUpdate { std::move(onUpdate) } {
            // Possibly respond on an invalid/empty std::function instance
        }

        // Non-virtual update function
        void update(Subject const& subject, StateTag property) {
            onUpdate(subject, property);
        }

    private:
        OnUpdate onUpdate;
    };


    class Person
    {
    public:
        enum StateChange
        {
            forenameChanged,
            surnameChanged,
            addressChanged
        };

        using PersonObserver = Observer<Person,StateChange>;

        explicit Person( std::string forename, std::string surname )
                : forename_{ std::move(forename) }
                , surname_{ std::move(surname) }
        {}

        bool attach( PersonObserver* observer )
        {
            auto [pos,success] = observers_.insert( observer );
            return success;
        }

        bool detach( PersonObserver* observer )
        {
            return ( observers_.erase( observer ) > 0U );
        }

        void notify( StateChange property )
        {
            for( auto iter=begin(observers_); iter!=end(observers_); )
            {
                auto const pos = iter++;
                (*pos)->update(*this,property);
            }
        }

        void forename(std::string newForename)
        {
            forename_ = std::move(newForename);
            notify( forenameChanged );
        }

        void surname(std::string newSurname )
        {
            surname_ = std::move(newSurname);
            notify( surnameChanged );
        }

        void address(std::string newAddress )
        {
            address_ = std::move(newAddress);
            notify( addressChanged );
        }

        [[nodiscard]]
        const std::string& forename() const {
            return forename_;
        }

        [[nodiscard]]
        const std::string& surname () const {
            return surname_;
        }

        [[nodiscard]]
        const std::string& address () const {
            return address_;
        }

    private:
        std::string forename_;
        std::string surname_;
        std::string address_;

        std::set<PersonObserver*> observers_;
    };

    void propertyChangedHandler(const Person& person,
                                Person::StateChange property)
    {
        if (property == Person::forenameChanged || property == Person::surnameChanged )
        {
            std::cout << "propertyChanged(): Forename --> " << person.forename() << std::endl;
        }
    }

    void test()
    {
        using PersonObserver = Observer<Person,Person::StateChange>;

        PersonObserver nameObserver(propertyChangedHandler);

        PersonObserver addressObserver([](const Person & person, Person::StateChange property){
            if (Person::addressChanged == property)
            {
                std::cout << "addressObserver: Address --> " << person.address() << std::endl;
            }
        });

        Person homer( "Homer"     , "Simpson" );
        Person marge( "Marge"     , "Simpson" );
        Person monty( "Montgomery", "Burns"   );

        // Attaching observers
        homer.attach( &nameObserver );
        marge.attach( &addressObserver );
        monty.attach( &addressObserver );


        // Updating information on Homer Simpson
        homer.forename( "Homer Jay");

        // Updating information on Marge Simpson
        marge.address( "712 Red Bark Lane, Henderson, Clark County, Nevada 89011" );

        // Updating information on Montgomery Burns
        monty.address( "Springfield Nuclear Power Plant" );

        // Detaching observers
        homer.detach( &nameObserver );
    }
}

namespace DesignPatterns::Decorator
{
    struct Money {
        uint64_t value{};
    };


    template<typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]]
    Money operator*(const Money& money, T factor) {
        return Money {static_cast<uint64_t>( money.value * factor )};
    }

    [[nodiscard]]
    constexpr Money operator+(const Money& lhs, const Money& rhs) noexcept {
        return Money{lhs.value + rhs.value};
    }

    std::ostream &operator<<(std::ostream &stream, const Money &money) {
        stream << money.value;
        return stream;
    }



    template<typename T>
    concept PricedItem = requires(T item) {
        { item.price() } -> std::same_as<Money>;
    };

    template<int taxRate, PricedItem Item>
    class Taxed : private Item {
    public:
        template<typename... Args>
        explicit Taxed(Args&& ... args): Item {std::forward<Args>(args)...} {
            // ....
        }

        [[nodiscard]]
        Money price() const {
            return Item::price() * (1.0 + (taxRate / 100));
        }
    };


    template<int discount, PricedItem Item>
    class Discounted {
    public:
        template<typename... Args>
        explicit Discounted(Args&& ... args): item{std::forward<Args>(args)...} {
            // ....
        }

        [[nodiscard]]
        Money price() const {
            return item.price() * (1.0 - (discount / 100));
        }

    private:
        Item item;
    };


    struct Ticket
    {
        Ticket(std::string name, Money price ): name_{ std::move(name) } , price_{ price } {
            // ....
        }

        [[nodiscard]]
        const std::string& name() const {
            return name_;
        }

        [[nodiscard]]
        Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };


    struct Book
    {
        Book(std::string name, Money price ): name_{ std::move(name) }, price_{ price }
        {}

        [[nodiscard]]
        std::string const& name() const {
            return name_;
        }

        [[nodiscard]] Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };


    void test()
    {
        Taxed<15, Discounted<20, Ticket>> item1 { "Core C++", Money{499} };
        Taxed<16, Discounted<21, Ticket>> item2 { "Core C++", Money{499} };
        Taxed<17, Discounted<22, Book>> item3 { "Core C++", Money{499} };

        [[maybe_unused]]
        const Money totalPrice1 = item1.price();  // Results in 459.08

        [[maybe_unused]]
        const Money totalPrice2 = item2.price();

        [[maybe_unused]]
        const Money totalPrice3 = item3.price();
    }
}

namespace DesignPatterns::TypeErasure_VoidType
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

namespace DesignPatterns::TagDispatching_RegisterIO
{

    class Read {};
    class Write {};
    class ReadWrite : public Read, public Write {};

    template <std::uint32_t Address, typename AccessType>
    class Register
    {
        volatile std::uint32_t* const m_reg = reinterpret_cast<volatile std::uint32_t* const>(Address);

        void write(std::uint32_t value, Write) noexcept {
            *m_reg = value;
        }

        [[nodiscard]]
        std::uint32_t read(Read) const noexcept {
            return *m_reg;
        }

    public:
        Register& operator= (const std::uint32_t value) noexcept {
            write(value, AccessType{});
            return *this;
        }

        operator std::uint32_t() const noexcept {
            return read(AccessType{});
        }
    };

    void TestAll()
    {
        Register<0x4000'0000, ReadWrite> r1;  /// OK
        r1 = 10;                              /// OK
        std::uint32_t value1 = r1;            /// OK

        Register<0x4000'0000, Write> r2;      /// OK
        r2 = 10U;                             /// OK
        // std::uint32_t value2 = r2;         /// ---> Compilation error

        Register<0x4000'0000, Read> r3;        /// OK
        // r3 = 10;                            /// ---> Compilation error
        std::uint32_t value3 = r3;             /// OK
    }
}

void DesignPatterns::TestAll()
{
    // Singleton::Test();

    // Strategy::Test();
    // Strategy_Text::Test();

    // CRTP::Test();

    // Chain_of_Responsibility::Test();

    // HouseFacade::Test();

    // Mediator::Test();

    // State::Test();
    // State_Visitor::Test();

    // Builder::Test_User();

    // Monostate::test();

    // Observer::test();

    // Decorator::test();

    // TypeErasure_VoidType::test();

    TagDispatching_RegisterIO::TestAll();
}
