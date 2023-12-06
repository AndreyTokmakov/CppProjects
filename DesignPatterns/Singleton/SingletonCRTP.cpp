/**============================================================================
Name        : SingletonCRTP.cpp
Created on  : 06.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SingletonCRTP.cpp
============================================================================**/

#include <iostream>

struct INonCopyable
{
    INonCopyable() = default;
    INonCopyable(const INonCopyable&) = delete;
    INonCopyable& operator=(const INonCopyable&) = delete;
};

template<typename Derived>
struct Singleton: private INonCopyable
{
    static Derived& getInstance()
    {
        static Derived instance {};
        return instance;
    }

protected:
    Singleton() = default;
};


struct FileManager: public Singleton<FileManager>
{
    FileManager() : Singleton<FileManager>{} {
        std::cout << "FileManager created\n";
    }
};

struct MemoryManager: public  Singleton<MemoryManager>
{
    MemoryManager() : Singleton<MemoryManager>{} {
        std::cout << "MemoryManager created\n";
    }
};


void SingletonCRTP_Test()
{
    FileManager& mgr1 = FileManager::getInstance();
    FileManager& mgr2 = FileManager::getInstance();

    MemoryManager& fileMgr1 = MemoryManager::getInstance();
    MemoryManager& fileMgr2 = MemoryManager::getInstance();
}