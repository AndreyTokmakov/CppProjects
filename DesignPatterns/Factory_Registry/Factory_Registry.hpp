/**============================================================================
Name        : Factory_Registry.hpp
Created on  : 08.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Factory_Registry.hpp
============================================================================**/

#ifndef CPPPROJECTS_FACTORY_REGISTRY_HPP
#define CPPPROJECTS_FACTORY_REGISTRY_HPP

namespace factory_registry
{
    void basic();
    void compiletimeExample();

    inline void TestAll()
    {
        // basic();
        compiletimeExample();
    }
}

#endif //CPPPROJECTS_FACTORY_REGISTRY_HPP