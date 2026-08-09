/**============================================================================
Name        : PubSub.hpp
Created on  : 08.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PubSub.hpp
============================================================================**/

#ifndef CPPPROJECTS_PUBSUB_HPP
#define CPPPROJECTS_PUBSUB_HPP

namespace published_subscriber
{
    void TestAll();
    namespace simple_subscriber_callback { void TestAll(); }
    namespace subscriber_different_events { void TestAll(); }
    namespace subscriber_different_topics_static { void TestAll(); }
    namespace subscriber_different_topics_defer_remove { void TestAll(); }
}

#endif //CPPPROJECTS_PUBSUB_HPP
