/**============================================================================
Name        : Numa.cpp
Created on  : 27.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Numa.cpp
============================================================================**/

#include "Numa.h"

#include <iostream>
#include <vector>


namespace
{
    template<typename T>
    std::ostream& operator<<(std::ostream& stream,
                             const std::vector<T>& vector)
    {
        for (const auto& v: vector)
            stream << v << ' ';
        return stream;
    }

    std::vector<int> EnumerateNumaNodes()
    {
        int maxNodes = numa_num_possible_nodes();
        std::vector<int> numaNodes;
        for (int i = 0; i < maxNodes; i++)
        {
            if (numa_bitmask_isbitset(numa_all_nodes_ptr, i))
                numaNodes.push_back(i);
        }
        return numaNodes;
    }

    void BindThreadToNumaNode(int numaNode)
    {
        struct bitmask * numaBitmask = numa_bitmask_alloc(numa_num_possible_nodes());
        numa_bitmask_setbit(numaBitmask, numaNode);
        numa_bind(numaBitmask);
        numa_bitmask_free(numaBitmask);
    }
}


void Numa::TestAll()
{
    const int32_t numaAvailable = numa_available();
    if (-1 == numaAvailable)
    {
        std::cerr << "NUMA is not available\n";
        return;
    }


    std::cout << "numa_max_possible_node    : " << numa_max_possible_node() << std::endl;
    std::cout << "numa_num_possible_nodes   : " << numa_num_possible_nodes() << std::endl;
    std::cout << "numa_max_node             : " << numa_max_node() << std::endl;
    std::cout << "numa_num_configured_nodes : " << numa_num_configured_nodes() << std::endl;
    std::cout << "numa_get_mems_allowed     : " << numa_get_mems_allowed() << std::endl;
    std::cout << "numa_num_configured_cpus  : " << numa_num_configured_cpus() << std::endl;
    std::cout << "numa_num_task_cpus        : " << numa_num_task_cpus() << std::endl;

    std::cout << "Numa nodes:        :" << std::endl;
    const std::vector<int> nodes = EnumerateNumaNodes();
    std::cout << nodes << std::endl;


};