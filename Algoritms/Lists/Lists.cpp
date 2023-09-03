//============================================================================
// Name        : LinkedListSimple.cpp
// Created on  : 03.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Linked list src class
//============================================================================

#include <iostream>
#include <algorithm>
#include <string>
#include <cassert>
#include <initializer_list>  // std::initializer_list
#include <optional>

#include <vector>
#include <list>
#include <forward_list>
#include <unordered_set>


#include "Lists.h"

namespace LinkedList
{
	// A Linked list Node 
	template<typename Type>
	class Node {
	public:
		Type data;
		Node* next;

	public:
		Node(Type value) : data(value), next(nullptr) {
		}

		Node(Type value, Node* nxt) : data(value), next(nxt) {
		}

	public: /** Operators reload. **/
		friend std::ostream& operator<<(std::ostream& stream, const Node<Type>& node);
	};

	template<typename Type>
	std::ostream& operator<<(std::ostream& os, const Node<Type>& node) {
		os << node.data;
		return os;
	}


	/////////////////////////////////////////////////////

	template<typename Type>
	Node<Type>* InitList(std::initializer_list<Type> args) {
		assert(std::size(args) > 0), "List expectes to have more than one element";
		Node<Type>* root = new Node<Type>(*std::begin(args)), * node = root;
		for (const Type* it = std::begin(args) + 1; it != std::end(args); ++it) {
			node->next = new Node(*it);
			node = node->next;
		}
		return root;
	}

    template<typename Type>
    Node<Type>* InitList(const std::vector<Type>& data) {
        assert(std::size(args) > 0), "List expectes to have more than one element";
        Node<Type>* root = new Node<Type>(data.front()), * node = root;
        for (size_t idx = 1; idx < data.size(); ++idx) {
            node->next = new Node(data[idx]);
            node = node->next;
        }
        return root;
    }

	template<typename T>
	void PrintList(const Node<T>* root, const std::string& text = "\n") {
		auto* node = const_cast<Node<T>*>(root);
		while (nullptr != node) {
			std::cout << node->data << " ";
			node = node->next;
		}
		std::cout << text;
	}


}


namespace LinkedList {

	void PrintListTest() {
		Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });
		PrintList(root);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	void Check_List_Is_Cycled(Node<T>* root) {
		Node<int>* slow = root, * fast = root;
        // size_t count = 0;
		while (nullptr != slow && nullptr != fast && nullptr != fast->next) {
			slow = slow->next;
			fast = fast->next->next;
            // ++count;
			if (slow == fast) {
				std::cout << " Loop detected" << std::endl;
                // std::cout << "count = " << count << std::endl;
				break;
			}
		}
	}

	template<typename T>
	void Check_List_Is_Cycled_2(Node<T>* root)
	{
        // size_t count = 0;
		std::unordered_set< Node<int>*> nodes;
		Node<int>* node = root;
		while (nullptr != node)  {
            // ++count;
			if (!nodes.insert(node).second)
			{
				std::cout << " Loop detected" << std::endl;
                // std::cout << "count = " << count << std::endl;
                return;
			}
			node = node->next;
		}
	}

	void CheckIfCycled() {
		Node<int>* root = new Node(1), * node = root;
		for (int i = 2; i < 100; i++) {
			node->next = new Node(i);
			node = node->next;
		}
		node->next = root;

		Check_List_Is_Cycled(root);
		Check_List_Is_Cycled_2(root);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<T>* __reverse_list(Node<T>* root) {
		Node<int>* prev = nullptr, * current = root, * next = nullptr;
		while (nullptr != current) {
			next = current->next;
			current->next = prev;
			prev = current;
			current = next;
		}
		return prev; // Return root
	}

	template<typename T>
	Node<T>* __reverse_list_2(Node<T>* node) {
		Node<int>* previous{ nullptr }, * next{ nullptr };
		while (nullptr != node) {
			next = node->next;
			node->next = previous;
			previous = node;
			node = next;
		}
		return previous; // Return root
	}

	void Reverse_Test() {
		{
			Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });

			PrintList(root);
			root = __reverse_list(root);
			PrintList(root);
		}
		{
			Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });

			PrintList(root);
			root = __reverse_list_2(root);
			PrintList(root);
		}
	}

	// ---------------------------------------------------------------------//

	template<typename T>
	Node<T>* _reserve_sublists(Node<T>* head) {
		Node<int>* next = head;

		while (nullptr != next) {
			if (0 == next->data % 2) {
			}
			next = next->next;
		}
		std::cout << std::endl;
		return head;
	}

	void Reverse_Sublists_Operations() {
		Node<int>* list = InitList({ 1, 2, 8, 9, 12, 16 });
		PrintList(list);
		Node<int>* result = _reserve_sublists(list);
		PrintList(result);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<T>* _make_reverse_list(Node<T>* root) {
		Node<int>* prev = new Node<int>(root->data);

		Node<int>* node = root->next;
		while (nullptr != node) {
			Node<int>* current = new Node<int>(node->data, prev);
			prev = current;
			node = node->next;
		}
		return prev;
	}

	void Make_Reversed_FromList() {
		Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });

		PrintList(root);
		root = _make_reverse_list(root);
		PrintList(root);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	bool check_if_palindrome(Node<T>* root)
	{
		std::forward_list<int> list;
		Node<int>* node = root;
		while (nullptr != node) {
			list.push_front(node->data);
			node = node->next;
		}

		auto iter = list.cbegin();
		node = root;
		while (nullptr != node) {
			if (node->data != *iter)
				return false;
			node = node->next;
			++iter;
		}
		return true;
	}

	//----------------------------------------------------------------------//


	void Check_If_Palindrom() {
		{
			Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });
			std::cout << std::boolalpha << check_if_palindrome(root) << std::endl;
		}

		{
			Node<int>* root = InitList({ 1,2,3,2,1 });
			std::cout << std::boolalpha << check_if_palindrome(root) << std::endl;
		}
	}

	template<typename T>
	size_t _get_length(Node<T>* root) {
		size_t len = 0;
		while (nullptr != root) {
			root = root->next;
			len++;
		}
		return len; // Return root
	}

	template<typename T>
	size_t _get_length_2(const Node<T>* root) {
		return nullptr == root ? 0 : 1 + _get_length_2(root->next);
	}

	void GetLength() {
		Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9 });
		PrintList(root);
		std::cout << "Length: " << _get_length(root) << std::endl;
		std::cout << "Length: " << _get_length_2(root) << std::endl;
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<T>* _find_Nth_element_from_the_end(Node<T>* root, int N) {
		Node<T>* node = root, * tmp = root;
		while (nullptr != root) {
			if (0 > N--)
				node = node->next;
			root = root->next;
		}
		return tmp == node ? nullptr : node;
	}

    template<typename T>
    Node<T>* _find_Nth_element_from_the_end_EX(Node<T>* root, int N) {
        Node<T>* node = root, *tmp = root;
        while (node && N--)
            node = node->next;
        while (node) {
            node = node->next;
            tmp = tmp->next;
        }
        return tmp;
    }

	template<typename T>
	std::optional<Node<T>*> _find_Nth_element_from_the_end_Opt(Node<T>* root, int N) {
		Node<T>* node = nullptr, * tmp = root;
		while (nullptr != root) {
			if (0 >= N--)
				node = (nullptr == node) ? tmp : node->next;
			root = root->next;
		}
		return nullptr == node ? std::nullopt : std::make_optional<Node<T>*>(node);
	}

	void Find_Nth_Element_FromTheEnd() {
		Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 });
		constexpr int K = 1;

		{
			Node<int>* result = _find_Nth_element_from_the_end(root, K);
			if (nullptr == result)
				std::cout << "None" << std::endl;
			else
				std::cout << result->data << std::endl;
		}
		{
			std::optional<Node<int>*> result = _find_Nth_element_from_the_end_Opt(root, K);
			if (true == result.has_value())
				std::cout << result.value()->data << std::endl;
			else
				std::cout << "None" << std::endl;
        }
        {
            Node<int>* result = _find_Nth_element_from_the_end_EX(root, K);
            if (nullptr == result)
                std::cout << "None" << std::endl;
            else
                std::cout << result->data << std::endl;
        }
	}

    //----------------------------------------------------------------------//

    template<typename T>
    Node<T>* _remove_N_Node_From_End(Node<T>* root, int N) {
        Node<T>* node = root, *tmp = root;
        while (node && N-- >= 0)
            node = node->next;
        while (node) {
            node = node->next;
            tmp = tmp->next;
        }

        tmp->next = tmp->next->next;
        return root;
    }

    void Remove_N_Node_From_End()
    {
        Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 });
        constexpr int N = 7;
        _remove_N_Node_From_End(root, N);

        PrintList(root);
    }

    //----------------------------------------------------------------------//

    void findMiddle(const Node<int>* root) {
        size_t steps = 0;
        const Node<int>* mid = root;
        while (nullptr != root) {
            root = root->next;
            if (0 == (steps++) % 2)
                mid = mid->next;
        }
        std::cout << mid->data << std::endl;
    }

    void findMiddle2(const Node<int>* root) {
        const Node<int>* slow = root, *fast = root;
        while (nullptr != fast && nullptr != fast->next) {
            fast = fast->next->next;
            slow = slow->next;
        }
        std::cout << slow->data << std::endl;
    }

    void FindMiddleElement() {
        Node<int>* root = InitList({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 });

        findMiddle(root);
        findMiddle2(root);

	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* merge_recursion(Node<T>* node1, Node<T>* node2) {
		if (nullptr == node1)
			return node2;
		else if (nullptr == node2)
			return node1;

		if (node1->data < node2->data) {
			node1->next = merge_recursion(node1->next, node2);
			return node1;
		}
		else {
			node2->next = merge_recursion(node1, node2->next);
			return node2;
		}
	}

	void Merge_Two_Lists() {
		Node<int>* list1 = InitList({ 0,2,4,6,8,10,12,14 });
		Node<int>* list2 = InitList({ 1,3,5,7,9,11,13,15 });

		PrintList(list1);
		PrintList(list2);
		Node<int>* result = merge_recursion(list1, list2);
		PrintList(result);
	}

	//----------------------------------------------------------------------//

	Node<int>* merge(Node<int>* node1, Node<int>* node2);

	Node<int>* merge2(Node<int>* node1, Node<int>* node2) {
		node1->next = merge(node1->next, node2);
		return node1;
	}

	Node<int>* merge(Node<int>* node1, Node<int>* node2) {
		if (nullptr == node1)
			return node2;
		else if (nullptr == node2)
			return node1;
		return node1->data < node2->data ? merge2(node1, node2) : merge2(node2, node1);
	}

	void Merge_Two_Lists_2() {
		Node<int>* list1 = InitList({ 0,2,4,6,8,10,12,14 });
		Node<int>* list2 = InitList({ 1,3,5,7,9,11,13,15 });

		PrintList(list1);
		PrintList(list2);
		Node<int>* result = merge(list1, list2);
		PrintList(result);
	}

    //----------------------------------------------------------------------//

    template<typename T>
    Node<int>* merge_recursion_non_recur(Node<T>* node1,
                                         Node<T>* node2)
    {
        Node<int>* root = nullptr;
        if (node1->data < node2->data) {
            root = node1;
            node1 = node1->next;
        } else {
            root = node2;
            node2 = node2->next;
        }

        Node<int>* node = root;
        while (node1 && node2)
        {
            if (node1->data < node2->data) {
                node->next = node1;
                node1 = node1->next;
            }
            else {
                node->next = node2;
                node2 = node2->next;
            }
            node = node->next;
        }

        return root;
    }

    void Merge_Two_Lists_NonRecursive()
    {
        Node<int>* list1 = InitList({ 0,2,4,6,8,10,12,14 });
        Node<int>* list2 = InitList({ 1,3,5,7,9,11,13,15 });

        const Node<int>* result = merge_recursion_non_recur(list1, list2);
        PrintList(result);
    }

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* _insert_into_sorted_list(Node<T>* root, T value) {
		Node<T>* node = root, * previous = nullptr, * newNode = new Node<T>(value);
		if (nullptr == node)
			return newNode;
		else if (node->data > value) {
			newNode->next = node;
			node = newNode;
			return node;
		}

		while (nullptr != node) {
			if (node->data > value) {
				newNode->next = node;
				previous->next = newNode;
				break;
			}
			previous = node;
			node = node->next;
		}
		return root;
	}

    void Insert_Into_Sorted_List() {
		{
			Node<int>* list = InitList({ 1,2,4,6,8,10,12,14 });
			PrintList(list);

			Node<int>* result = _insert_into_sorted_list(list, 9);
			PrintList(result);
		}
		{
			Node<int>* list = InitList({ 1,2,4,6,8,10,12,14 });
			PrintList(list);

			Node<int>* result = _insert_into_sorted_list(list, 0);
			PrintList(result);
		}
		{
			Node<int>* list = nullptr;
			PrintList(list);

			Node<int>* result = _insert_into_sorted_list(list, 11);
			PrintList(result);
		}
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* _insert_into_sorted_list_2(Node<T>* root, T value) {
		Node<T>* newNode = new Node<T>(value);
		if (nullptr == root || root->data > value) {
			newNode->next = root;
			return newNode;
		}

		Node<T>* previous = root, * next = root->next;
		while (nullptr != next) {
			if (next->data > value) {
				newNode->next = next;
				previous->next = newNode;
				return root;
			}

			previous = next;
			next = previous->next;
		}
		return root;
	}

	void Insert_Into_Sorted_List_2() {
		Node<int>* list = InitList({ 1,2,4,6,8,10,12,14 });
		PrintList(list);

		Node<int>* result = _insert_into_sorted_list_2(list, 9);
		PrintList(result);
	}

    //----------------------------------------------------------------------//

    template<typename T>
    Node<int>* _insert_into_sorted_list_3(Node<T>* root, T value)
    {
        Node<T>* const new_node = new Node<T>(value);
        if (nullptr == root || root->data > value) {
            new_node->next = root;
            return new_node;
        }

        Node<T> *node = root->next, *prev = root;
        while (nullptr != node && node->data <= value) {
            prev = node;
            node = node->next;
        }

        prev->next = new_node;
        new_node->next = node;

        return root;
    }

    void Insert_Into_Sorted_List_3()
    {
        Node<int>* list = InitList({ 1,2,4,6,8,10,12,14 });
        PrintList(list);

        Node<int>* result = _insert_into_sorted_list_3(list, 9);
        PrintList(result);
    }

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>*  _delete_nodes(Node<T>* root, const T value) {
		Node<T>* next = root->next, * prev = nullptr;
		while (nullptr != root && value == root->data)
			root = root->next;

		prev = root;
		next = nullptr != root ? root->next : nullptr;

		while (nullptr != next) {
			if (value != next->data) {
				prev->next = next;
				prev = prev->next;
			}
			next = next->next;
		}
		return root;
	}

	void DeleteNodes_ByValue()
    {
        for (const std::vector<int>& data: std::vector<std::vector<int>> {
                { 5,1,5,5,2,4,5,5,5,7,8,9 }, { 5,1,5,5,2,4,5,5,5,7,8,9 }, { 5,5,5 }
        })
        {
            Node<int>* list = InitList(data);
            PrintList(list, "  -->  ");
            PrintList(_delete_nodes(list, 5), "\n\n");
        }
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* _delete_nodes_2(Node<T>* root, const T value) {
		while (nullptr != root && value == root->data)
			root = root->next;

		Node<T>* previous = root, * current = root ? root->next : nullptr;
		while (nullptr != current) {
			if (value == current->data)
				previous->next = current->next;
			else
				previous = current;
			current = previous->next;
		}
		return root;
	}

	void DeleteNodes_ByValue2()
    {
        for (const std::vector<int>& data: std::vector<std::vector<int>> {
                { 5,1,5,5,2,4,5,5,5,7,8,9 }, { 5,1,5,5,2,4,5,5,5,7,8,9 }, { 5,5,5 }
        })
        {
            Node<int>* list = InitList(data);
            PrintList(list, "  -->  ");
            PrintList(_delete_nodes_2(list, 5), "\n\n");
        }
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* _delete_nodes_3(Node<T>* root, const T value) {
		Node<T>* previous = root, * current = root ? root->next : nullptr;
		while (nullptr != current) {
			if (value == current->data)
				previous->next = current->next;
			else
				previous = current;
			current = previous->next;
		}
		return root && root->data == value ? root->next : root;
	}

	void DeleteeNodes_ByValue3() {
		Node<int>* list = InitList({ 5,5,5,1,5,5,2,4,5,5,5,7,8,9 });
		PrintList(list, "  -->  ");
		list = _delete_nodes_3(list, 5);
		PrintList(list);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* _delete_node_by_pos(Node<T>* root, size_t pos) {
		// TODO: return root->next IF pos == 0
		Node<T>* node = root;
		while (nullptr != node && node->next != nullptr) {
			pos--;
			if (0 == pos) {
				node->next = node->next->next;
				return root;
			}
			node = node->next;
		}
		return root;
	}

	template<typename T>
	Node<int>* _delete_node_by_pos_2(Node<T>* root, size_t pos) {
		// TODO: return root->next IF pos == 0
		Node<T>* previous = root, *current = root ? root->next : nullptr;
		while (nullptr != current) {
			pos--;
			if (0 == pos) {
				previous->next = current->next;
				return root;
			}
			previous = current;
			current = previous->next;
		}
		return root;
	}

	template<typename T>
	Node<int>* _delete_node_by_pos_3(Node<T>* root, size_t pos) {
		if (0 == pos)
			return root->next;

		Node<T>* previous = root;
		while (nullptr != root) {
			if (pos--)
				previous = root;
			else {
				previous->next = root->next;
				break;
			}
			root = previous->next;
		}
	}


	void DeleteeNodes_By_Position() {
		{
			Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
			PrintList(list, "  -->  ");
			list = _delete_node_by_pos(list, 1);
			PrintList(list);
		}
		{
			Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
			PrintList(list, "  -->  ");
			list = _delete_node_by_pos(list, 8);
			PrintList(list);
		}
		{
			Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
			PrintList(list, "  -->  ");
			list = _delete_node_by_pos_2(list, 1);
			PrintList(list);
		}
		{
			Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
			list = nullptr;
			PrintList(list, "  -->  ");
			_delete_node_by_pos_3(list, 1);
			PrintList(list);
		}
	}

	//----------------------------------------------------------------------------------

	template<typename T>
	Node<int>* _delete_K_node_from_the_end(Node<T>* root, int K) {
		Node<T>* node = root, * tmp{ nullptr };
		while (nullptr != node) {
			if (K-- == 0)
				tmp = root;
			else if (K < 0) {
				tmp = tmp->next;
			}
			node = node->next;
		}
		tmp->next = tmp->next->next;
		return root;
	}

	void Delete_K_Element_From_the_End() {
		Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
		PrintList(list, "  -->  ");
		list = _delete_K_node_from_the_end(list, 2);
		PrintList(list);
	}

	//----------------------------------------------------------------------------------


	template<typename T>
	Node<int>* _move_to_the_end(Node<T>* root, const T value) {
		while (nullptr != root && value == root->data)
			root = root->next;

		Node<T>* tail = nullptr;
		Node<T>* current = root, * previous = nullptr;
		while (nullptr != current) {
			if (value == current->data) {
				previous->next = current->next;
				if (nullptr == tail)
					tail = current;
				else
					tail->next = current;
			}
			else {
				previous = current;
			}
			current = previous->next;
		}

		std::cout << previous->data << std::endl;

		return root;
	}


	void MoveAllOccurrencesToTheEnd_TEST() {
		Node<int>* list = InitList({ 2,2,2,2,3,4,5,6,7,2,2,2,8,9,10 });
		PrintList(list);
		list = _move_to_the_end(list, 2);
		PrintList(list);
	}

	//----------------------------------------------------------------------//

	template<typename T>
	Node<int>* delete_duplices(Node<T>* root) {
		Node<T>* prev = root, * next = nullptr != root ? root->next : nullptr;
		std::unordered_set<T> tmp = { root->data };

		while (nullptr != next) {
			if (true == tmp.insert(next->data).second) {
				prev->next = next;
				prev = prev->next;
			}
			next = next->next;
		}
		return root;
	}

	template<typename T>
	void remove_duplicates(Node<T>* node) {
		Node<T>* previous = nullptr;
		std::unordered_set<T> tmp;

		while (nullptr != node) {
			if (false == tmp.insert(node->data).second)
				previous->next = node->next;
			else
				previous = node;
			node = previous->next;
		}
	}

	template<typename T>
	void remove_duplicates_2(Node<T>* node) {
		Node<T>* previous = nullptr;
		std::unordered_set<T> tmp;

		while (nullptr != node) {
			if (false == tmp.insert(node->data).second) {
				previous->next = node->next;
				node = previous->next;
			}
			previous = node;
			if (nullptr == node)
				return;
			node = node->next;
		}
	}


	void Delete_Duplicates() {
		std::cout << "--------------------- Test1: ------------------\n" << std::endl;
		{
			Node<int>* list = InitList({ 1,2,3,3,4,5,6,6,7,8 });
			PrintList(list, "  -->  ");
			PrintList(delete_duplices(list), "\n\n");
		}
		std::cout << "\n--------------------- Test2: ------------------\n" << std::endl;
		{
			Node<int>* list = InitList({ 1,2,3,3,4,5,6,6,7,8 });
			PrintList(list, "  -->  ");
			remove_duplicates(list);
			PrintList(list);
		}
		std::cout << "\n--------------------- Test3: ------------------\n" << std::endl;
		{
			Node<int>* list = InitList({ 1,2,3,3,4,5,6,6,7,8 });
			PrintList(list, "  -->  ");
			remove_duplicates_2(list);
			PrintList(list);
		}
	}

	//----------------------------------------------------------------------//

	template<typename T>
	void remove_duplicates_sorted(Node<T>* node) {
		Node<T>* previous = nullptr;
		while (nullptr != node) {
			if (node->next && node->data == node->next->data)
				previous->next = node->next;
			else
				previous = node;
			node = previous->next;
		}
	}

	void Delete_Duplicates_InSortedList() {
		Node<int>* list = InitList({ 1,2,3,3,4,5,6,6,6,6,7,8 });
		PrintList(list, "  -->  ");
		remove_duplicates_sorted(list);
		PrintList(list);
	}

	//----------------------------------------------------------------------//

	void Reverse_Even_Subarrays()
    {
		Node<int>* list = InitList({ 2, 18, 24, 3, 5, 7, 9, 6, 12 }); // -> 24, 18, 2, 3, 5, 7, 9, 12, 6
		Node<int>* node = list;

		Node<int>* start = nullptr, * end = nullptr;
		std::vector<int> values;
		while (nullptr != node) {
			std::cout << node->data << std::endl;

			if (0 == node->data % 2) {
				values.push_back(node->data);
				if (nullptr == start)
					start = node;
				end = node;
			}
			else {
				std::reverse(values.begin(), values.end());
				for (int i : values) {
					start->data = i;
					start = start->next;
				}
				start = nullptr;
				end = nullptr;
				values.clear();
			}
			node = node->next;
		}

		std::reverse(values.begin(), values.end());
		for (int i : values) {
			start->data = i;
			start = start->next;
		}
	}

	//---------------------------------------------------------------------------------//

	template<typename T>
	Node<int>* __move_all_occurrences_to_th_end(Node<T>* root, T value) {
		assert(nullptr != root);
		Node<int>* start = nullptr, * tmp1 = nullptr;
		Node<int>* tail = nullptr, * tmp2 = nullptr;
		while (nullptr != root) {
			if (value != root->data) {
				if (nullptr == start) {
					start = root;
					tmp1 = start;
				}
				else {
					tmp1->next = root;
					tmp1 = tmp1->next;
				}
			}
			else {
				if (nullptr == tail) {
					tail = root;
					tmp2 = tail;
				}
				else {
					tmp2->next = root;
					tmp2 = tmp2->next;
				}
			}
			root = root->next;
		}
		tmp2->next = nullptr;
		tmp1->next = tail;
		return start;
	}

    template<typename T>
    Node<int>* __move_all_occurrences_to_th_end_GOOD_BEST(Node<T>* root, const T value)
    {
        Node<T> *node1 {nullptr}, *start { nullptr }, *node2 {nullptr}, *tail {  nullptr };
        while (root)
        {
            if (value != root->data) {
                if (!node1)
                    start = node1 = root;
                else {
                    node1->next = root;
                    node1 = node1->next;
                }
            }
            else {
                if (!node2)
                    tail = node2 = root;
                else {
                    node2->next = root;
                    node2 = node2->next;
                }
            }
            root = root->next;
        }

        node2->next = nullptr;
        node1->next = tail;

        return start;
    }



	template<typename T>
	Node<int>* __move_all_occurrences_to_th_end_2(Node<T>* root, T value) {
		assert(nullptr != root);
		Node<int>* start = nullptr, * tmp1 = nullptr;
		Node<int>* tail = nullptr, * tmp2 = nullptr;
		while (nullptr != root) {
			if (value != root->data) {
				if (nullptr == start) {
					start = root;
					tmp1 = start;
				}
				else {
					tmp1->next = root;
					tmp1 = tmp1->next;
				}
			}
			else {
				if (nullptr == tail) {
					tail = root;
					tmp2 = tail;
				}
				else {
					tmp2->next = root;
					tmp2 = tmp2->next;
				}
			}
			root = root->next;
		}
		tmp2->next = nullptr;
		tmp1->next = tail;
		return start;
	}

    template<typename T>
    Node<int>* __move_all_occurrences_to_th_end_TEST(Node<T>* root,
                                                     T value)
    {
        Node<T>* tailRoot, *tail = nullptr;
        if (value == root->data) {
            tail = (tailRoot = root);
            root = root->next;
        }

        Node<T>* previous = root, * current = root ? root->next : nullptr;
        while (nullptr != current) {
            if (value == current->data) {
                previous->next = current->next;
                if (!tail)
                    tail = (tailRoot = current);
                else {
                    tail->next = current;
                    tail = tail->next;
                }
            }
            else
                previous = current;
            current = previous->next;
        }

        tail->next = nullptr;
        previous->next = tailRoot;
        return root;
    }

	void MoveAllOccurrencesToTheEnd()
    {
        /*
		{
			Node<int>* list = InitList({ 2,2,2,2,3,4,5,6,7,2,2,2,8,9,10 });
			PrintList(list);
			Node<int>* result = __move_all_occurrences_to_th_end(list, 2);
			PrintList(result);
		}
        */

        /** Best solution so far**/
        {
            Node<int>* list = InitList({2,2,2,1,1});
            PrintList(list);
            Node<int>* result = __move_all_occurrences_to_th_end_GOOD_BEST(list, 2);
            PrintList(result);
        }


        /*
		{
			Node<int>* list = InitList({ 2,2,2,2,3,4,5,6,7,2,2,2,8,9,10 });
			PrintList(list);
			Node<int>* result = __move_all_occurrences_to_th_end_2(list, 2);
			PrintList(result);
		}
        {
            Node<int>* list = InitList({ 2,2,2,2,3,4,5,6,7,2,2,2,8,9,10 });
            //PrintList(list);
            Node<int>* result = __move_all_occurrences_to_th_end_TEST(list, 2);
            PrintList(result);

        }
        */
	}
}

namespace LinkedList::JustTests {
	//--------------------------------------------------------------------------------------------//

	// Delete node by values
	template<typename T>
	Node<int>* _delete_node_(Node<T>* root, const T value) {
		while (nullptr != root && root->data == value)
			root = root->next;

		Node<int>* previous = root, * current = root ? root->next : nullptr;
		while (nullptr != current) {
			if (value == current->data)
				previous->next = current->next;
			else
				previous = current;
			current = previous->next;
		}
		return root;
	}

	// Delete node by values
	template<typename T>
	Node<int>* _reverse_(Node<T>* node) {
		Node<int>* previous = nullptr, * next = node;
		while (nullptr != node) {
			next = node->next;

			node->next = previous;
			previous = node;

			node = next;
		}
		return previous;
	}

	template<typename T>
	Node<T>* _reverse_test(Node<T>* root) {
		// void  _reverse_test(Node<T>* root) {
		assert(nullptr != root);
		Node<int>* previous = nullptr, * next = nullptr, * node = root;
		while (nullptr != node) {
			next = node->next;
			node->next = previous;
			previous = node;
			node = next;
		}
		return previous;
	}

	template<typename T>
	Node<T>* _reverse_test_rec(Node<T>* root, Node<T>* prev = nullptr) {
		root->next->next = prev;
		return root->next;
	}

	void SOME_TESTS() {
#if 0   // Delete list test //
		Node<int>* list = InitList({ 5,1,5,5,2,4,5,5,5,7,8,9 });
		PrintList(list);
		list = _delete_node_(list, 5);
		PrintList(list);
#endif

#if 0   // Reverse list test 1 //
		Node<int>* list = InitList({ 1,2,3,4,5 });
		PrintList(list);
		list = _reverse_(list);
		PrintList(list);
		std::cout << "Done" << std::endl;
#endif

#if 0   // Reverse list test 2 //
		Node<int>* list = InitList({ 1,2,3,4,5,6,7,8,9 });
		PrintList(list);
		list = _reverse_test(list);
		PrintList(list);
		std::cout << "Done" << std::endl;
#endif



        auto* root = new Node<int>(0);;
        Node<int>* node = root;

        for (int i = 1; i < 10; ++i)
        {
            node->next = new Node<int>(i);
            node = node->next;
        }

        PrintList(root);

	}
}


void LinkedList::TEST_ALL()
{
	// LinkedList::PrintListTest();

    // LinkedList::CheckIfCycled();

	// LinkedList::Reverse_Test();
	// LinkedList::Reverse_Sublists_Operations();

	// LinkedList::Make_Reversed_FromList();

	// LinkedList::GetLength();

	// LinkedList::Check_If_Palindrom();

	// LinkedList::Merge_Two_Lists();
	// LinkedList::Merge_Two_Lists_2();
	// LinkedList::Merge_Two_Lists_NonRecursive();

	// LinkedList::Insert_Into_Sorted_List();
	// LinkedList::Insert_Into_Sorted_List_2();
	// LinkedList::Insert_Into_Sorted_List_3();

	// LinkedList::DeleteNodes_ByValue();
	// LinkedList::DeleteNodes_ByValue2();
	// LinkedList::DeleteeNodes_ByValue3();
	// LinkedList::DeleteeNodes_By_Position();
	// LinkedList::DeleteeNodes_By_Position();
	// LinkedList::Delete_K_Element_From_the_End();

	// LinkedList::MoveAllOccurrencesToTheEnd_TEST();

	// LinkedList::Delete_Duplicates();
	// LinkedList::Delete_Duplicates_InSortedList();

    // LinkedList::FindMiddleElement();
    // LinkedList::Remove_N_Node_From_End();
    // LinkedList::Find_Nth_Element_FromTheEnd();
	// LinkedList::MoveAllOccurrencesToTheEnd();

	// LinkedList::Reverse_Even_Subarrays();

	// JustTests::SOME_TESTS();



}
