/**============================================================================
Name        : Hazard_Pointers.cpp
Created on  : 21.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Hazard_Pointers.cpp
============================================================================**/

#include "Hazard_Pointers.hpp"

#include <atomic>
#include <optional>


#if 0

#include <hazard_pointer>

namespace
{
    struct Node : std::hazard_pointer_obj_base<Node>
    {
        int value;
        Node* next;
    };
    std::atomic<Node*> head{nullptr};
}

std::atomic<Node*> head{nullptr};

void push(int value)
{
    Node* n = new Node;
    n->value = value;
    n->next = head.load();
    while (!head.compare_exchange_weak(n->next, n)) {
        //
    }
}

std::optional<int> pop()
{
    std::hazard_pointer hp = std::make_hazard_pointer();
    Node* old = hp.protect(head);
    while (old) {
        Node* next = old->next;        // safe: old is protected
        if (head.compare_exchange_weak(old, next)) {
            int value = old->value;
            old->retire();             // freed later, not now
            return value;
        }
        old = hp.protect(head);        // CAS failed: protect the new head
    }
    return std::nullopt;
}

#endif



void memory::hazard_pointers::TestAll()
{

}
