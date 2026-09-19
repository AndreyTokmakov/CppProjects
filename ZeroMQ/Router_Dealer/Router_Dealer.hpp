/**============================================================================
Name        : Router_Dealer.h
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer.h
============================================================================**/

#ifndef ROUTER_DEALER_H
#define ROUTER_DEALER_H

namespace router_dealer
{
    void TestAll();

    namespace experiments { void TestAll(); }
    namespace simple { void TestAll(); }
    namespace demo_one { void TestAll(); }
    namespace poller_timeout { void TestAll(); }

    namespace asynch_1 { void TestAll(); }
    namespace asynch_2 { void TestAll(); }
    namespace asynch_server_class { void TestAll(); }
}

#endif //ROUTER_DEALER_H