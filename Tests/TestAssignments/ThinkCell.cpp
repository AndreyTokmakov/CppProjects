/**============================================================================
Name        : ThinkCell.cpp
Created on  : 11.06.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThinkCell test task
============================================================================**/

#include "TestAssignments.h"


#include <iostream>
#include <map>

namespace ThinkCell
{
    struct Int
    {
        int value {0};
    };


    template<typename K, typename V>
    class interval_map
    {
    public: // FIXME
        friend void IntervalMapTest();

        V m_valBegin;
        std::map<K, V> m_map;
    public:
        // constructor associates whole range of K with val
        explicit interval_map(const V &val) : m_valBegin(val) {
            //
        }

        // Assign value val to interval [keyBegin, keyEnd).
        // Overwrite previous values in this interval.
        // Conforming to the C++ Standard Library conventions, the interval
        //
        // includes keyBegin, but excludes keyEnd.
        // If !( keyBegin < keyEnd ), this designates an empty interval, and assign must do nothing.
        void assign(const K& keyBegin, const K& keyEnd, const V& val) {
            if (!( keyBegin < keyEnd ))
                return;

            const auto valX = this->operator[](keyEnd);
            const auto [iterStart, okStart] = m_map.insert_or_assign(keyBegin, val);
            const auto [iterEnd, okEnd] = m_map.insert_or_assign(keyEnd, valX);

            m_map.erase(std::next(iterStart), iterEnd);
        }

        // look-up of the value associated with key
        V const &operator[](const K& key) const {
            auto it = m_map.upper_bound(key);
            if (m_map.begin() == it) {
                return m_valBegin;
            } else {
                return (--it)->second;
            }
        }
    };

    // Many solutions we receive are incorrect. Consider using a randomized test
    // to discover the cases that your implementation does not handle correctly.
    // We recommend to implement a test function that tests the functionality of
    // the interval_map, for example using a map of int intervals to char.

    // The member interval_map<K,V>::m_valBegin holds the value that is associated with all keys less than the first key in m_map.


    // TODO:
    //  Test for 'M.m_valBegin=='A' - whole range assosiated with A
    //  Key: int, double, float, string ?
    //    negatives ??
    //  Intervals:
    //  - insert middle
    //  - insert overlap: lower
    //  - insert overlap: upper
    //  - insert overlap: from lover and upper
    //  - insert overlap: re-assign interval
    //  - insert overlap: 1 - 2 ... diff just 1


    template<typename Key, typename Val>
    Val operatorDummy(const interval_map<Key, Val> &map, const Key &key) {
        auto it = map.m_map.upper_bound(key);
        if (map.m_map.begin() == it) {
            return map.m_valBegin;
        } else {
            std::cout << "iter: [" << it->first << ", " << it->second << "]\n";
            return (--it)->second;
        }
    }

    template<typename Key, typename Val>
    void printMap(const interval_map<Key, Val> &map, bool debug = false)
    {
        if (debug)
        {
            std::cout << " ------------------- Size = " << map.m_map.size() << "-------------------\n";
            for (const auto &[k, v]: map.m_map) {
                std::cout << k << " -> " << v << std::endl;
            }
            std::cout << " ------------------------------------------------------------------------\n";
        }
    }

    size_t errorsCount { 0 };

    void printTestSummary() {
        if (errorsCount > 0) {
            std::cout << "Some tests has failed\n";
        }
        else {
            std::cout << "Success\n";
        }
    }

    template<typename Key, typename Val>
    void testKeyValue(const interval_map<Key, Val> &map,
                      const Key& key, const Val& valExpected, bool debug = false)
    {
        const auto& actualValue = map[key];
        if (valExpected != actualValue) {
            ++errorsCount;
            std::cerr << "ERROR: map[" << key << "] != " << valExpected
                      << ". Actual value: " << actualValue << std::endl;
        }
        else {
            if (debug)
                std::cout << "OK: map[" << key << "] == " << valExpected << std::endl;
        }
    }

    template<typename Key, typename Val>
    void testKeyValueRange(const interval_map<Key, Val> &map,
                           const Key& keyBegin, const Key& keyEnd, const Val& valExpected, bool debug = false)
    {
        for (Key key = keyBegin; key < keyEnd; ++key) {
            testKeyValue(map, key, valExpected, debug);
        }
    }

    void BordersCheckTests()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 15, 'A');
        imap.assign(10, 20, 'B');

        imap.assign(10, 10, 'Z');
        imap.assign(11, 10, 'Z');
        imap.assign(-1, -3, 'Z');
        imap.assign(11, 10, 'Z');
        imap.assign(0, 0, 'Z');

        testKeyValueRange(imap, -10, 5,'X', false);
        testKeyValueRange(imap,  5, 10,'A', false);
        testKeyValueRange(imap, 10, 20,'B', false);
        testKeyValueRange(imap, 20, 30,'X', false);
    }

    void singleElementInterval_One()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(1, 2, 'A');
        printMap(imap);

        testKeyValueRange(imap, -10, 1, 'X');
        testKeyValue(imap, 1, 'A');
        testKeyValueRange(imap, 1, 2, 'A');
        testKeyValue(imap, 2, 'X');
    }

    void singleElementInterval_Two()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(1, 2, 'A');
        imap.assign(2, 3, 'A');
        printMap(imap);

        testKeyValue(imap, 0, 'X');
        testKeyValue(imap, 1, 'A');
        testKeyValue(imap, 2, 'A');
        testKeyValueRange(imap, 1, 3, 'A');
        testKeyValueRange(imap, 3, 31, 'X');
    }

    void singleElementInterval_Two_Ex()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(1, 2, 'A');
        imap.assign(2, 3, 'B');
        printMap(imap);

        testKeyValue(imap, 0, 'X');
        testKeyValue(imap, 1, 'A');
        testKeyValue(imap, 2, 'B');

        testKeyValueRange(imap, -10, 1, 'X');
        testKeyValueRange(imap, 1, 2, 'A');
        testKeyValueRange(imap, 2, 3, 'B');
        testKeyValueRange(imap, 3, 31, 'X');
    }

    void singleElementInterval_Two_Separated()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(1, 2, 'A');
        imap.assign(4, 5, 'B');

        printMap(imap);

        testKeyValue(imap, 0, 'X');
        testKeyValue(imap, 1, 'A');

        testKeyValue(imap, 2, 'X');
        testKeyValue(imap, 3, 'X');

        testKeyValue(imap, 4, 'B');

        testKeyValueRange(imap, -10, 1, 'X');
        testKeyValueRange(imap, 1, 2, 'A');
        testKeyValueRange(imap, 2, 4, 'X');
        testKeyValueRange(imap, 4, 5, 'B');
        testKeyValueRange(imap, 5, 31, 'X');
    }


    void justOneInterval()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(1, 10, 'A');

        printMap(imap, true);
        testKeyValueRange(imap, -3, 1,'X', false);
        testKeyValueRange(imap, 1, 10,'A', false);
        testKeyValue(imap, 10, 'X');
        testKeyValue(imap, 110,'X');
    }

    void justOne_BiggerThenExising()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 10, 'A');

        printMap(imap);
        testKeyValueRange(imap, -3, 5,'X', false);
        testKeyValueRange(imap, 5, 10,'A', false);
        testKeyValue(imap, 10, 'X');
        testKeyValue(imap, 110,'X');


        imap.assign(2, 25, 'A');

        printMap(imap);
        testKeyValueRange(imap, -3, 2,'X', false);
        testKeyValueRange(imap, 2, 25,'A', false);
        testKeyValueRange(imap, 25, 33,'X', false);
    }

    void TwoIntervals_NonIntersect()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 10, 'A');
        imap.assign(15, 20, 'B');

        testKeyValueRange(imap, -10, 5,'X', false);
        testKeyValueRange(imap,  5, 10,'A', false);
        testKeyValueRange(imap, 10, 15,'X', false);
        testKeyValueRange(imap, 15, 20,'B', false);
        testKeyValueRange(imap, 20, 30,'X', false);
    }

    void TwoIntervals_Intersect()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 15, 'A');
        imap.assign(10, 20, 'B');

        testKeyValueRange(imap, -10, 5,'X', false);
        testKeyValueRange(imap,  5, 10,'A', false);
        testKeyValueRange(imap, 10, 20,'B', false);
        testKeyValueRange(imap, 20, 30,'X', false);
    }


    void ThreeIntervals_NonIntersect()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 10, 'A');
        imap.assign(15, 20, 'B');
        imap.assign(25, 40, 'C');

        testKeyValueRange(imap, -10, 5,'X', false);
        testKeyValueRange(imap,  5, 10,'A', false);
        testKeyValueRange(imap, 10, 15,'X', false);
        testKeyValueRange(imap, 15, 20,'B', false);
        testKeyValueRange(imap, 20, 25,'X', false);
        testKeyValueRange(imap, 25, 40,'C', false);
        testKeyValueRange(imap, 40, 100,'X', false);
    }

    void justOne_BiggerThenExising_Two()
    {
        interval_map<int, char> imap {'X'};
        testKeyValue(imap, 1, 'X');

        imap.assign(5, 10, 'A');
        imap.assign(15, 20, 'B');

        testKeyValueRange(imap, -10, 5,'X', false);
        testKeyValueRange(imap,  5, 10,'A', false);
        testKeyValueRange(imap, 10, 15,'X', false);
        testKeyValueRange(imap, 15, 20,'B', false);
        testKeyValueRange(imap, 20, 30,'X', false);
    }

    void twoIntervals_OverlapLeft()
    {
        interval_map<int, char> imap {'X'};

        imap.assign(4, 10, 'A');
        printMap(imap,true);

        testKeyValueRange(imap, -3, 4,'X', false);
        testKeyValueRange(imap, 4, 10,'A',false);
        testKeyValueRange(imap, 20, 30,'X', false);

        imap.assign(1, 6, 'B');
        printMap(imap,true);

        testKeyValueRange(imap, -3, 1,'X', false);
        testKeyValueRange(imap, 1, 6,'B',false);
        testKeyValueRange(imap, 6, 10,'A',false);
        testKeyValueRange(imap, 10, 30,'X', false);
    }

    void reAssignInterval()
    {
        interval_map<int, char> imap {'X'};
        printMap(imap);
        //testKeyValue(imap, 1);


        imap.assign(1, 10, 'A');
        printMap(imap);
        // testKeyValueRange(imap, 1);
        // testKeyValueRange(imap, 9);
        // testKeyValueRange(imap, 10); // FIXME: Bug


        imap.assign(1, 10, 'B');
        printMap(imap);
    }

    void insertIntervalInMiddle()
    {
        interval_map<int, char> imap {'X'};
        //printMap(imap);

        imap.assign(1, 20, 'A');
        printMap(imap);

        testKeyValueRange(imap, -10, 0,'X', false);
        testKeyValueRange(imap, 1, 20,'A', false);
        testKeyValueRange(imap, 20, 30,'X', false);


        imap.assign(3, 10, 'B');

        printMap(imap);

        testKeyValueRange(imap, -10, 0,'X', false);
        testKeyValueRange(imap, 1, 3,'A', false);
        testKeyValueRange(imap, 3, 10,'B', false);
        testKeyValueRange(imap, 10, 20,'A', false);
        testKeyValueRange(imap, 20, 30,'X', false);
    }
}

void ThinkCell_Tests()
{
    ThinkCell::singleElementInterval_One();
    ThinkCell::singleElementInterval_Two();
    ThinkCell::singleElementInterval_Two_Ex();
    ThinkCell::singleElementInterval_Two_Separated();

    ThinkCell::justOneInterval();
    ThinkCell::justOne_BiggerThenExising();
    ThinkCell::justOne_BiggerThenExising_Two();

    ThinkCell::twoIntervals_OverlapLeft();
    ThinkCell::reAssignInterval();
    ThinkCell::insertIntervalInMiddle();

    ThinkCell::TwoIntervals_NonIntersect();
    ThinkCell::ThreeIntervals_NonIntersect();

    ThinkCell::TwoIntervals_Intersect();

    ThinkCell::BordersCheckTests();

    ThinkCell::printTestSummary();
}
