/**============================================================================
Name        : WatchDirectoryChanges.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : WatchDirectoryChanges.cpp
============================================================================**/

#include "WatchDirectoryChanges.h"

#include <iostream>
#include <array>

#include <sys/inotify.h>
#include "../common.h"


namespace
{
    /* Maximum number of events to process at a time: */
    constexpr size_t maxEvents {1024};

    /* We assume that the length of the file name does not exceed 16 characters */
    constexpr size_t nameLength {16};

    /* Size of the Event structure: */
    constexpr size_t eventSize {sizeof(inotify_event)};

    /* Size of the Event structure */
    constexpr size_t bufferSize {maxEvents * (nameLength + eventSize)};
}


namespace WatchDirectoryChanges
{
    void NotifyTest()
    {
        int monitor = inotify_init1(0);
        if (-1 == monitor) {
            std::cout << "Failed to init Monitor. Error = " << errno << std::endl;
            return;
        }

        constexpr std::string_view path { R"(/tmp/dir_for_testing)" };
        int wd = inotify_add_watch(monitor, path.data(), IN_CLOSE | IN_MODIFY);
        if (-1 == wd) {
            std::cout << "Failed to create watcher. Error = " << errno << std::endl;
            close(monitor);
            return;
        }

        std::array<char, bufferSize> buffer {};
        long i = 0, length = 0;
        while (true) {
            i = 0;
            length = read(monitor, buffer.data(), bufferSize);
            if (0 > length) {
                std::cout << "read() failed. Error = " << errno << std::endl;
                break;
            }

            while (length > i) {
                const auto *event = reinterpret_cast<inotify_event*>(&buffer[i]);
                if (event->len) {
                    if ( event->mask & IN_CLOSE) {
                        if (event->mask & IN_ISDIR)
                            std::cout << "The directory '" << event->name << "' was closed.\n";
                        else
                            std::cout << "The file '" << event->name << "' was closed with ID: " << event->wd << std::endl;
                    }
                    if ( event->mask & IN_MODIFY) {
                        if (event->mask & IN_ISDIR)
                            std::cout << "The directory '" << event->name << "' was modified.\n";
                        else
                            std::cout << "The file '" << event->name << "' was modified with ID: " << event->wd << std::endl;
                    }
                    i += bufferSize + event->len;
                }
            }
        }
        inotify_rm_watch(monitor, wd);
        close(monitor);
    }
}




void WatchDirectoryChanges::TestAll()
{
    NotifyTest();
}
