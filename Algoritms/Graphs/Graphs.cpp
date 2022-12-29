//============================================================================
// Name        : Graphs.cpp
// Created on  : 25.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities class 
//============================================================================

#include "Graphs.h"

#include <iostream>
#include <algorithm>
// #include <limits>

#include <list>
#include <deque>
#include <map>
#include <chrono>
#include <vector>


namespace {

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
		for (const T& entry : vect)
			stream << entry << ' ';
		return stream;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::list<T>& vect) {
		for (const T& entry : vect)
			stream << entry << ' ';
		return stream;
	}

#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}
}

namespace Graphs::Tests {

	class Graph {
	private:
		using value_type = int;
		std::map<value_type, std::vector<value_type>> nodes;

	public:

		void addEdge(value_type v,
					 value_type w) {
			nodes[v].push_back(w);
		}

		[[nodiscard]]
		inline size_t size() const noexcept {
			return this->nodes.size();
		}

		const std::vector<value_type>& operator[](size_t index) const {
			return this->nodes.at(index);
		}
	};

	void BFS(const Graph& graphs, int s)
	{
		std::vector<bool> visited(graphs.size(), false);

		visited[s] = true;
		std::deque<int> queue { s }; // deque contains 1-st node

		while (!queue.empty()) {
			s = queue.front();
			queue.pop_front();
			std::cout << s << " ";

			for (const auto& nodes = graphs[s]; int it: nodes) {
				if (!visited[it]) {
					visited[it] = true;
					queue.push_back(it);
				}
			}
		}
	}


	void BFS_Test() {
		// Create a graph given in the above diagram 
		Graph graphs;
		graphs.addEdge(0, 1);
		graphs.addEdge(0, 2);
		graphs.addEdge(1, 2);
		graphs.addEdge(2, 0);
		graphs.addEdge(2, 3);
		graphs.addEdge(3, 3);

		std::cout << "Following is Breadth First Traversal (starting from vertex 2):" << std::endl;
		BFS(graphs, 2);
		std::cout << std::endl;
	}
}

namespace Graphs::DFS_Tests {

	class Graph
	{
	public:
		using value_type = int;

		std::map<value_type, bool> visited;
		std::map<value_type, std::vector<value_type>> nodes;

		void addEdge(value_type v, value_type w) {
			nodes[v].push_back(w); // Add w to v�s list.
		}

		void DFS(value_type v) {
			// Mark the current node as visited and print it
			visited[v] = true;
			std::cout << v << " ";

			// Recur for all the vertices adjacent to this current node
			for (const value_type id : nodes[v])
				if (!visited[id])
					DFS(id);
		}
	};

	void Test()
	{
		// Create a graph given in the above diagram
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 9);
		g.addEdge(1, 2);
		g.addEdge(2, 0);
		g.addEdge(2, 3);
		g.addEdge(9, 3);

		std::cout << "Following is Depth First Traversal (starting from vertex 2) \n";
		g.DFS(2);
	}
}



namespace Graphs::DFS_Tests2 {

    class Graph
    {
    public:
        std::map<int, std::vector<int>> nodes;

        void addEdge(int v, int w) {
            nodes[v].push_back(w); // Add w to v�s list.
        }

        void dfs(int v,  std::vector<bool>& visited) {
            // Mark the current node as visited and print it
            visited[v] = true;
            std::cout << v << " ";

            for (const std::vector<int>& childs = nodes[v]; const int id: childs)
                if (!visited[id])
                    dfs(id, visited);
        }

        void DFS(int v) {
            // FIXME: Size() is wrong we should use MIN and MAX node id's
            std::vector<bool> visited(nodes.size() + 1, false);
            dfs(v, visited);
        }
    };

    void Test()
    {
        // Create a graph given in the above diagram
        Graph g;
        g.addEdge(0, 1);
        g.addEdge(0, 9);
        g.addEdge(1, 2);
        g.addEdge(2, 0);
        g.addEdge(2, 3);
        g.addEdge(9, 3);

        std::cout << "Following is Depth First Traversal (starting from vertex 2) \n";
        g.DFS(2);
    }
}


namespace Graphs::Find_Mother_Vertex {

	class Graph
	{
	public:
		std::map<int, bool> visited;
		std::map<int, std::vector<int>> nodes;

		void addEdge(int v, int w) {
			nodes[v].push_back(w);
		}

		void DFS_FindMotherVertex(int mother, int child, int v) {
			visited[v] = true;
			if (child == v)
				std::cout << mother << " -> " << child << std::endl;

			for (const auto id : nodes[v])
				if (!visited[id])
					DFS_FindMotherVertex(v, child, id);

			visited[v] = false;
		}
	};

	void Test()
	{
		// Create a graph given in the above diagram
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 9);
		g.addEdge(1, 2);
		g.addEdge(2, 0);
		g.addEdge(2, 3);
		g.addEdge(9, 3);


		// Start from one vertex, for example:  0 -->1 and search for node = 3 
		g.DFS_FindMotherVertex(0, 3, 1);
	}
}


namespace Graphs::Detect_Cycle {

	class Graph
	{
	public:
		std::map<int, bool> visited;
		std::map<int, bool> track;
		std::map<int, std::vector<int>> nodes;

		void addEdge(int v, int w) {
			nodes[v].push_back(w);
		}

		void DFS_IsCycled(int v) {
			if (visited[v] == false) {
				visited[v] = true;
				track[v] = true;

				std::cout << v << "  | " << std::boolalpha << track[v] << std::endl;

				for (const auto id : nodes[v])
					if (visited[id] == false)
						DFS_IsCycled(id);

				track[v] = false;
				visited[v] = false;
			}
		}

		void IsCycled() {
			for (const auto& [k, v] : nodes)
				DFS_IsCycled(k);
		}
	};

	void Test()
	{
		// Create a graph given in the above diagram
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 9);
		g.addEdge(1, 2);
		g.addEdge(2, 0);
		g.addEdge(2, 3);
		g.addEdge(9, 3);
		g.addEdge(3, 0);

		g.IsCycled();
	}
}



namespace Graphs::Find_All_Paths {

	class Graph
	{
	public:
        using ValueType = int;

		std::map<ValueType, bool> visited;
		std::map<ValueType, std::vector<ValueType>> nodes;
		std::vector<ValueType> path;

		void addEdge(ValueType v, ValueType w) {
			nodes[v].push_back(w);
		}

		void FindPaths(ValueType v,
                       ValueType node_to_find) {
			visited[v] = true;
			path.push_back(v);

			if (v == node_to_find) { // Print full path
				std::cout << path << std::endl;
			}
			else {
				for (const ValueType id : nodes[v])
					if (!visited[id])
						FindPaths(id, node_to_find);
			}

			// To get the entire list of availableWorkers paths, you need to mark the nodes as not viewed
			// (after the completion of the recursion on this node - otherwise we will go even partially along this 
			// path) + delete these nodes and the path itself
			path.pop_back();
			visited[v] = false;
		}
	};

	void Test()
	{
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 2);
		g.addEdge(0, 3);
		g.addEdge(2, 0);
		g.addEdge(2, 1);
		g.addEdge(1, 3);


		g.FindPaths(2, 3);
	}
}

namespace Graphs::Find_Longest_Path {

	class Graph
	{
	public:
		std::map<int, bool> visited;
		std::map<int, std::vector<int>> nodes;

		std::vector<int> path;
		std::vector<int> max_path;

		void addEdge(int v, int w) {
			nodes[v].push_back(w);
		}

		void FindPaths(int v, int node_to_find) {
			visited[v] = true;
			path.push_back(v);

			if (v == node_to_find) {
				if (path.size() > max_path.size())
					max_path.assign(path.begin(), path.end());
			}
			else {
				for (const auto id : nodes[v])
					if (!visited[id])
						FindPaths(id, node_to_find);
			}

			// To get the entire list of availableWorkers paths, you need to mark the nodes as not viewed
			// (after the completion of the recursion on this node - otherwise we will go even partially along this 
			// path) + delete these nodes and the path itself
			path.pop_back();
			visited[v] = false;
		}

		void print_max() {
			std::cout << max_path << std::endl;
		}
	};

	void Test()
	{
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 2);
		g.addEdge(0, 3);
		g.addEdge(2, 0);
		g.addEdge(2, 1);
		g.addEdge(1, 3);


		g.FindPaths(2, 3);
		g.print_max();
	}
}


namespace Graphs::Find_Shortest_Path {

    class Graph
    {
    public:
        std::map<int, std::vector<int>> nodes;
        std::vector<int> path;
        std::vector<int> shortest;
        int maxId {0};

        void addEdge(int from, int to) {
            nodes[from].push_back(to);
            maxId = std::max(std::max(from, to), maxId) ;
        }

        void FindPaths(int v, int node_to_find)
        {
            path.clear();
            shortest.clear();
            std::vector<bool> visited(maxId + 1, false);

            findPath(v, node_to_find, visited);
        }

        void findPath(int from, int to, std::vector<bool>& visited) {
            visited[from] = true;
            path.push_back(from);

            if (from == to) {
                if (shortest.empty() || shortest.size() > path.size())
                    shortest.assign(path.begin(), path.end());
            }
            else {
                for (const auto id : nodes[from])
                    if (!visited[id])
                        findPath(id, to, visited);
            }

            path.pop_back();
            visited[from] = false;
        }

        void print_max() {
            std::cout << "shortest_path = " << shortest << std::endl;
        }
    };

    void Test()
    {
        Graph g;
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);
        g.addEdge(2, 0);
        g.addEdge(2, 1);
        g.addEdge(1, 3);


        g.FindPaths(2, 3);
        g.print_max();
    }
}


namespace Graphs::Find_Shortest_and_Longest_Path {

	class Graph
	{
	public:
		std::map<int, bool> visited;
		std::map<int, std::vector<int>> nodes;

		std::vector<int> path;

		std::vector<int> longest_path;
		std::vector<int> shortest_path;

		void addEdge(int v, int w) {
			nodes[v].push_back(w);
		}

		void FindPaths(int v, int node_to_find) {
			visited[v] = true;
			path.push_back(v);

			if (v == node_to_find) {
				if (shortest_path.empty())
					shortest_path.assign(path.begin(), path.end());
				else if (shortest_path.size() > path.size())
					shortest_path.assign(path.begin(), path.end());

				if (path.size() > longest_path.size())
					longest_path.assign(path.begin(), path.end());
			}
			else {
				for (const auto id : nodes[v])
					if (false == visited[id])
						FindPaths(id, node_to_find);
			}

			// To get the entire list of availableWorkers paths, you need to mark the nodes as not viewed
			// (after the completion of the recursion on this node - otherwise we will go even partially along this 
			// path) + delete these nodes and the path itself
			path.pop_back();
			visited[v] = false;
		}

		void print_max() {
			std::cout << "longest_path = " << longest_path << std::endl;
			std::cout << "shortest_path = " << shortest_path << std::endl;
		}
	};

	void Test()
	{
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 2);
		g.addEdge(0, 3);
		g.addEdge(2, 0);
		g.addEdge(2, 1);
		g.addEdge(1, 3);


		g.FindPaths(2, 3);
		g.print_max();
	}
}

namespace Graphs::Find_MinimumNumberEdges {

	class Graph
	{
	public:
		std::map<int, bool> visited;
		std::map<int, std::vector<int>> nodes;

		int path = 0;
		// int minPath = std::numeric_limits<int>::max();
		int minPath = INT64_MAX;

		void addEdge(int v, int w) {
			nodes[v].push_back(w);
		}

		size_t findMinimumNumberEdges(int v, int node_to_find) {
			visited[v] = true;
			++path;

			if (v == node_to_find) {
				if (minPath > path)
					minPath = path;
			}
			else {
				for (const auto id : nodes[v])
					if (false == visited[id])
						findMinimumNumberEdges(id, node_to_find);
			}

			--path;
			visited[v] = false;
			return minPath - 1;
		}


	};

	void Test()
	{
		Graph g;
		g.addEdge(0, 1);
		g.addEdge(0, 7);
		g.addEdge(1, 7);
		g.addEdge(1, 2);
		g.addEdge(2, 3);
		g.addEdge(2, 5);
		g.addEdge(2, 8);
		g.addEdge(3, 4);
		g.addEdge(3, 5);
		g.addEdge(4, 5);
		g.addEdge(5, 6);
		g.addEdge(6, 7);
		g.addEdge(7, 8);


		auto edges = g.findMinimumNumberEdges(1, 5);
		std::cout << edges << std::endl;
	}
}



namespace Graphs_Perf::Find_Shortest_Path {

    class Graph
    {
    public:
        std::map<int, std::vector<int>> nodes;
        std::vector<int> path;
        std::vector<int> shortest;
        int maxId {0};

        void addEdge(int from, int to) {
            nodes[from].push_back(to);
            maxId = std::max(std::max(from, to), maxId) ;
        }

        void FindPaths(int v, int node_to_find)
        {
            path.clear();
            shortest.clear();
            std::vector<bool> visited(maxId + 1, false);

            findPath(v, node_to_find, visited);
        }

        void findPath(int from, int to, std::vector<bool>& visited) {
            visited[from] = true;
            path.push_back(from);

            if (from == to) {
                if (shortest.empty() || shortest.size() > path.size())
                    shortest.assign(path.begin(), path.end());
            }
            else {
                for (const auto id : nodes[from])
                    if (!visited[id])
                        findPath(id, to, visited);
            }

            path.pop_back();
            visited[from] = false;
        }

        void print_max() {
            std::cout << "shortest_path = " << shortest << std::endl;
        }
    };

    void Test()
    {
        Graph g;
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);
        g.addEdge(2, 0);
        g.addEdge(2, 1);
        g.addEdge(1, 3);


        g.FindPaths(2, 3);
        g.print_max();
    }

    void PerfTest()
    {
        Graph g;
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);
        g.addEdge(2, 0);
        g.addEdge(2, 1);
        g.addEdge(1, 3);

        {
            START_TIME_MEASURE
            constexpr int reps = 100'000'000;
            // constexpr int reps = 1;
            for (int i = 0; i < reps; ++i) {
                g.FindPaths(2, 3);
                // g.print_max();
            }
            STOP_TIME_MEASURE
        }
    }
}

namespace WeightGraphs::Find_Shortest_Path {

    template<typename T = int>
    struct Edge {
        T id {};
        T weight {};

        Edge(T id, T weight): id {id}, weight {weight} {
        }
    };

    template<typename T = int>
    std::ostream& operator<<(const Edge<T>& odge, std::ostream& stream){
        return stream;
    }


    class Graph
    {
    public:
        std::map<int, std::vector<Edge<int>>> nodes;
        std::vector<int> path;
        std::vector<int> shortest;
        int maxId {0};

        void addEdge(int from, Edge<int> to) {
            nodes[from].push_back(to);
            maxId = std::max(std::max(from, to.id), maxId) ;
        }

        void FindPaths(int v, int node_to_find)
        {
            path.clear();
            shortest.clear();
            std::vector<bool> visited(maxId + 1, false);

            findPath({v, 0}, node_to_find, visited);
        }

        void findPath(const Edge<int>& edge,
                      int node_to_find,
                      std::vector<bool>& visited) {
            visited[edge.id] = true;
            path.push_back(edge.id);

            if (edge.id == node_to_find) {
                if (shortest.empty() || shortest.size() > path.size())
                    shortest.assign(path.begin(), path.end());
            }
            else {
                for (const auto& entry : nodes[edge.id])
                    if (!visited[entry.id])
                        findPath(entry, node_to_find, visited);
            }

            path.pop_back();
            visited[edge.id] = false;
        }

        void printPath()
        {
            auto findWeight = [&](int from , int to) -> Edge<int> {
                const std::vector<Edge<int>>& edges = nodes[from];
                return *std::find_if(edges.cbegin(), edges.cend(), [&](const auto& entry) {
                    return entry.id == to;
                });
            };

            int from = shortest.front();
            std::cout << from;
            for (size_t i = 1; i < shortest.size(); ++i) {
                Edge<int> edge = findWeight(from, shortest[i]);
                std::cout << " --> (" << edge.weight << ") --> " << edge.id;
                from = shortest[i];
            }
        }
    };

    void Test()
    {
        Graph g;
        g.addEdge(0, {1, 0});
        g.addEdge(0, {2, 0});
        g.addEdge(0, {3, 1});
        g.addEdge(2, {0, 3});
        g.addEdge(2, {1, 0});
        g.addEdge(1, {3, 0});


        g.FindPaths(2, 3);
        g.printPath();
    }
}



void Graphs::TEST_ALL()
{
	// Tests::BFS_Test();

	// DFS_Tests::Test();
    // DFS_Tests2::Test();

    // Find_Mother_Vertex::Test();

	// Detect_Cycle::Test();

	Find_All_Paths::Test();
	// Find_Longest_Path::Test();
	// Find_Shortest_Path::Test();
	// Find_Shortest_and_Longest_Path::Test();

	// Find_MinimumNumberEdges::Test();

    // Graphs_Perf::Find_Shortest_Path::Test();
    // Graphs_Perf::Find_Shortest_Path::PerfTest();

    // WeightGraphs::Find_Shortest_Path::Test();
};
