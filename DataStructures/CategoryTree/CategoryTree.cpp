//============================================================================
// Name        : CategoryTree.h
// Created on  : 26.05.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ CategoryTree
//============================================================================

#include "CategoryTree.h"

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <unordered_map>

namespace CategoryTree::TreeOne
{

    template<typename T = std::string>
    struct Node {
        using KeyType = T;
        using ListIter = typename std::list<Node<KeyType>>::iterator;

        static_assert(!std::is_same_v<KeyType, void>, "ERROR: KeyType can not be void");

        template<typename ... Args>
        explicit Node(Args&& ... params): name (std::forward<Args>(params) ...) {

        }

    private:
        using NodeIterComparator = decltype([](const auto& first , const auto& second) {
            return first->name < second->name ;
        });

        template<typename M>
        friend class CategoryTree;

        KeyType name {};
        std::set<ListIter, NodeIterComparator> children {};
    };


    template<typename T = std::string>
    void PrintChildren(const Node<T>& node, std::string offset = std::string{}) {
        std::cout << offset << node.name << std::endl;
        for (const auto& ch: node.children) {
            std::cout << offset;
            PrintChildren(*ch, offset + " ");
        }
    }


    template<typename T = std::string>
    class CategoryTree final {
    public:
        using KeyType = T;
        using ListIter = typename std::list<Node<KeyType>>::iterator;

        std::list<Node<KeyType>> items {};
        std::unordered_map<KeyType, ListIter> cache;

    public:
        void addCategory(std::string&& childName,
                         std::string&& parentName = std::string{}) {
            auto [childCacheIter, childInserted] = cache.emplace(childName, items.end());
            if (childInserted) {
                childCacheIter->second = items.insert(items.end(), Node(childName));
            } else {
                // TODO: Error??
            }

            // TODO: use try_emplace ??
            auto [parentCacheIter, parentInserted] = cache.emplace(parentName, items.end());
            if (parentInserted) {
                parentCacheIter->second = items.insert(items.end(), Node(parentName));
            } else {
                // TODO: Error??
            }

            parentCacheIter->second->children.insert(childCacheIter->second);
        }


        void getChildren(const T& name, std::string offset = std::string{}) const {
            if (auto iter = cache.find(name); cache.end() != iter) {
                std::cout << offset << iter->second->name << std::endl;
                for (const auto& ch: iter->second->children) {
                    std::cout << offset;
                    getChildren(ch->name , offset + " ");
                }
            }
        }
    };

}

void CategoryTree::TEST_ALL()
{
    TreeOne::CategoryTree tree {};
    /*
    tree.addCategory("X", "A");
    tree.addCategory("Y", "A");
    tree.addCategory("X", "B");
    tree.addCategory("Y", "B");
    */


    /*
    tree.addCategory("B", "A");
    tree.addCategory("X", "A");
    tree.addCategory("C", "B");
    tree.addCategory("C1", "B");
    tree.addCategory("D", "C");

    auto a = tree.cache["A"];
    PrintChildren(*a);
    */


    tree.addCategory("X", "A");
    tree.addCategory("Y", "A");
    tree.addCategory("Y", "A");
    tree.addCategory("Y", "A");

    tree.addCategory("B", "Y");
    tree.addCategory("C", "Y");

    tree.addCategory("B", "X");
    tree.addCategory("C", "X");

    tree.getChildren("A");
}

