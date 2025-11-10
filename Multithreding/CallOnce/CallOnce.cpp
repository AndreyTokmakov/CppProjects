//============================================================================
// Name        : CallOnce.cpp
// Created on  : 22.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : std::call_once src
//============================================================================

#include <iostream>
#include <future>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

#include "../Utilities/Utilities.h"
#include "CallOnce.h"

namespace CallOnce::SimpleTests {


    void Test0() {
        std::once_flag instance_flag;

        auto initialize = []() {
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            THREAD_INFO << "Initt OK!" << std::endl;
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back([&instance_flag, &initialize]() { {
                THREAD_INFO << "Started" << std::endl;
                std::call_once(instance_flag, initialize);
                THREAD_INFO << "Done" << std::endl;
            }});
        for (auto& T : jobs)
            T.join();
    }

    class Handler {
    public:
        Handler() {
            THREAD_INFO << " Handler:Handler(). Should call only ONCE" << std::endl;
        }
    };

    std::shared_ptr<Handler> instance;
    std::once_flag instance_flag;

    void create_handler() {
        instance.reset(new Handler());
    }

    void thread_func() {
        THREAD_INFO << "Started" << std::endl;
        std::call_once(instance_flag, create_handler);
        THREAD_INFO << "Done" << std::endl;
    }

    void Test() {
        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(thread_func);
        for (auto& T : jobs)
            T.join();
    }

    void Test2()
    {
        THREAD_INFO << " started" << std::endl;
        std::shared_ptr<Handler> instance;
        std::once_flag instance_flag;

        auto create_handler = [&instance]()-> void {
            instance.reset(new Handler());
        };
        auto worker = [&instance_flag, &create_handler]()-> void {
            THREAD_INFO << " started" << std::endl;
            std::call_once(instance_flag, create_handler);
        };

        std::vector<std::future<void>> jobs;
        for (int i = 0; i++ < 3;)
            jobs.emplace_back(std::async(worker));
        for (auto& T : jobs)
            T.wait();
    }

    void Test3() {
        std::once_flag flag_once;

        const auto simple_do_once = [&flag_once]()->void {
            THREAD_INFO << "Thread started" << std::endl;
            std::call_once(flag_once, []() {
                THREAD_INFO << " Simple example: called once" << std::endl;
            });
            THREAD_INFO << "Thread done" << std::endl;
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(simple_do_once);
        for (auto& T : jobs)
            T.join();
    }

    void Test_DiffecrentHandlers_OneFlag() {
        std::once_flag flag_once;

        auto initialyze = []() {
            THREAD_INFO << "***** Some uniqe initialization function!! ********" << std::endl;
        };

        const auto handler1 = [&]()->void {
            THREAD_INFO << "Thread1 started" << std::endl;
            std::call_once(flag_once, initialyze);
            THREAD_INFO << "Thread1 done" << std::endl;
        };

        const auto handler2 = [&]()->void {
            THREAD_INFO << "Thread2 started" << std::endl;
            std::call_once(flag_once, initialyze);
            THREAD_INFO << "Thread2 done" << std::endl;
        };

        std::vector<std::future<void>> jobs;

        jobs.emplace_back(std::async(handler1));
        jobs.emplace_back(std::async(handler2));
        jobs.emplace_back(std::async(handler1));
        jobs.emplace_back(std::async(handler2));
        jobs.emplace_back(std::async(handler1));
        jobs.emplace_back(std::async(handler2));
        jobs.emplace_back(std::async(handler1));
        jobs.emplace_back(std::async(handler2));

        for (auto& T : jobs)
            T.wait();
    }
}

namespace CallOnce::TestWithException {

    void Test() {
        std::once_flag flag_once;

        const auto may_throw_function = [](bool do_throw)->void {
            if (do_throw) {
                THREAD_INFO << "throw: call_once will retry" << std::endl; // this may appear more than once
                throw std::exception();
            }
            THREAD_INFO << "Didn't throw, call_once will not attempt again" << std::endl;// guaranteed once
        };

        const auto do_once = [&may_throw_function, &flag_once](bool do_throw)->void {
            try {
                std::call_once(flag_once, may_throw_function, do_throw);
            }
            catch (...) {
            }
        };

        {
            std::vector<bool> states = { true, true, true, true, true, true, false, true, true, true, true };
            std::vector<std::thread> jobs;
            for (bool b : states)
                jobs.emplace_back(do_once, b);
            for (auto& T : jobs)
                T.join();
        }
    }
}

namespace CallOnce::SingletonTests_NoSync {

    class SingletonTest {
    private:
        static std::atomic<bool> is_constructed;
        static SingletonTest* instance;

    private:
        static void Initialyze() {
            SingletonTest::instance = new SingletonTest();
            SingletonTest::is_constructed = true;
        }

    public:
        static SingletonTest* getInstance() {
            //THREAD_INFO << " SingletonTest::getInstance()" << std::endl;
            if (false == SingletonTest::is_constructed) {
                if (nullptr == SingletonTest::instance) {
                    Initialyze();
                }
            }
            return SingletonTest::instance;
        }

    protected:
        SingletonTest() {
            THREAD_INFO << " ********** SingletonTest::SingletonTest() **********" << std::endl;
        }
    };

    std::atomic<bool> SingletonTest::is_constructed{ false };
    SingletonTest* SingletonTest::instance = nullptr;

    //////////////////////////////////////////////////


    void Test() {
        THREAD_INFO << " > From the main thread." << std::endl;

        const auto task = []()-> void {
            THREAD_INFO << " Task stated" << std::endl;

            [[maybe_unused]]
            SingletonTest* T = SingletonTest::getInstance();
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(task);
        for (auto& T : jobs)
            T.join();

    }
}


namespace CallOnce::SingletonTests_Mutex {

    // Mutex
    std::mutex mtx;

    class SingletonTest {
    private:
        static std::atomic<bool> is_constructed;
        static SingletonTest* instance;

    private:
        static void Initialyze() {
            // THREAD_INFO << " ******** Initialyze ************" << std::endl;
            SingletonTest::instance = new SingletonTest();
            SingletonTest::is_constructed = true;
        }

    public:
        static SingletonTest* getInstance() {
            // THREAD_INFO << " SingletonTest::getInstance()" << std::endl;
            if (false == SingletonTest::is_constructed) {
                std::lock_guard<std::mutex> lock(mtx);
                if (nullptr == SingletonTest::instance) {
                    Initialyze();
                }
            }
            return SingletonTest::instance;
        }

    protected:
        SingletonTest() {
            THREAD_INFO << " ********** SingletonTest::SingletonTest() **********" << std::endl;
        }
    };

    std::atomic<bool> SingletonTest::is_constructed { false };
    SingletonTest* SingletonTest::instance = nullptr;

    //////////////////////////////////////////////////


    void Test() {
        THREAD_INFO << " > From the main thread." << std::endl;
        const auto task = []()-> void {
            THREAD_INFO << " Task stated" << std::endl;

            [[maybe_unused]]
            SingletonTest* T = SingletonTest::getInstance();
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(task);
        for (auto& T : jobs)
            T.join();
    }
}


namespace CallOnce::SingletonTests_CallOnce {

    class SingletonTest {
    private:
        static bool is_constructed;
        static SingletonTest* instance;
        static std::once_flag flag_once;

    private:
        static void Initialyze() {
            // THREAD_INFO << " ******** Initialyze ************" << std::endl;
            SingletonTest::instance = new SingletonTest();
            SingletonTest::is_constructed = true;
        }

    public:
        static SingletonTest* getInstance() {
            // THREAD_INFO << " SingletonTest::getInstance()" << std::endl;
            if (false == SingletonTest::is_constructed) {
                if (nullptr == SingletonTest::instance) {
                    std::call_once(flag_once, SingletonTest::Initialyze);
                }
            }
            return SingletonTest::instance;
        }

    protected:
        SingletonTest() {
            THREAD_INFO << " ********** SingletonTest::SingletonTest() **********" << std::endl;
        }
    };

    bool SingletonTest::is_constructed { false };
    SingletonTest* SingletonTest::instance = nullptr;
    std::once_flag SingletonTest::flag_once;

    //////////////////////////////////////////////////


    void Test() {
        THREAD_INFO << " > From the main thread." << std::endl;
        const auto task = []()-> void {
            THREAD_INFO << " Task stated" << std::endl;

            [[maybe_unused]]
            SingletonTest* T = SingletonTest::getInstance();
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(task);
        for (auto& T : jobs)
            T.join();
    }
}

namespace CallOnce::CallClassMethod {

    class Worker
    {
    private:
        std::once_flag connection_init_flag;

        void open_connection() {
            THREAD_INFO << "*** [Opeping connection] ****" << std::endl;
        }

    public:
        void send_data() {
            std::call_once(connection_init_flag, &Worker::open_connection, this);
            THREAD_INFO << "Send data" << std::endl;
        }

        std::string receive_data()
        {
            std::call_once(connection_init_flag, &Worker::open_connection, this);
            THREAD_INFO << "Send data" << std::endl;
            return "Some_data";
        }
    };

    void Test() {
        Worker worker;

        const auto producer = [&worker]()->void {
            THREAD_INFO << "Producer started" << std::endl;
            worker.send_data();
        };

        const auto consumer = [&worker]()->void {
            THREAD_INFO << "Consumer started" << std::endl;
            worker.receive_data();
        };

        std::vector<std::future<void>> jobs;
        jobs.emplace_back(std::async(producer));
        jobs.emplace_back(std::async(consumer));

        for (auto& T : jobs)
            T.wait();
    }
}

namespace CallOnce::CallClassMethod_Static {

    class WSInitializerSimple final {
    private:
        /** To ensure that we call init_ws_library() only once. **/
        static inline std::once_flag init_flag { };
        static inline std::atomic_uint32_t counter { 0 };

    public:
        WSInitializerSimple() {
            std::cout << "WSInitializer()" << std::endl;
            std::call_once(WSInitializerSimple::init_flag, &WSInitializerSimple::init_ws_library, this);
            ++counter;
        }

        ~WSInitializerSimple() {
            std::cout << "~WSInitializerSimple()" << std::endl;
            if (0 == --counter) {
                clean_up();
            }
        }

    private:
        void init_ws_library() {

            std::cout << "init_ws_library()" << std::endl;
        }

        void clean_up() {

            std::cout << "Clean UP WS library" << std::endl;
        }
    };

    /////////////////////////////////////////////////////


    void Test() {
        WSInitializerSimple a1;
        WSInitializerSimple a2;
        WSInitializerSimple a3;
    }
}

/*
namespace CallOnce::AccessResource
{
    struct GlobalResource
    {
        static constexpr inline std::string_view headerPath = "/some/path/header";
        static constexpr inline std::string_view bodyPath = "/some/path/body";

        struct Header { };
        struct Body { };

        static const Header& getHeader() {
            std::call_once(flag, load, headerPath);
            return instance->header;
        }

        static const Body& getBody() {
            std::call_once(flag, load, bodyPath);
            return instance->body;
        }

    private:
        Header header;
        Body body;

        static void load(std::string_view path)
        {
            std::cout << "Loading resource from " << path << std::endl;
        }

        static inline std::once_flag flag;
        static inline std::unique_ptr<GlobalResource> instance { nullptr };
    };


    int Test() {
        std::vector<std::jthread> runners;
        std::generate_n(std::back_inserter(runners), 4,[]{
            return std::jthread([]{
                auto& header = GlobalResource::getHeader();
                // process header

                auto& body = GlobalResource::getBody();
                // process body
            });
        });
    }
};
*/

void CallOnce::TEST_ALL() {
    // SimpleTests::Test0();

    // SimpleTests::Test();
    // SimpleTests::Test2();
    // SimpleTests::Test3();
    // SimpleTests::Test_DiffecrentHandlers_OneFlag();

    // TestWithException::Test();

    // SingletonTests_NoSync::Test();
    // SingletonTests_Mutex::Test();
    // SingletonTests_CallOnce::Test();

    CallClassMethod::Test();

    // CallClassMethod_Static::Test();

    // AccessResource::Test();
};
