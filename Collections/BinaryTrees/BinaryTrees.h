#pragma once

template <class NodeType>
class Node {
protected:
	/** Node data. **/
	NodeType data;

protected:
	/** Pointer to the left child element. **/
	Node* left;
	/** Pointer to the right child element. **/
	Node* right;
	/** Pointer to the parent element. **/
	Node* parent;

public:
	/** Node constructor. **/
	Node(const NodeType& data);
	/** Node constructor. **/
	NodeType& getData() const;
};

template<class NodeType>
Node<NodeType>::Node(const NodeType& data) {
	this->data = data;
	this->left = nullptr;
	this->right = nullptr;
}

template <class NodeType>
NodeType& Node <NodeType>::getData() const {
	return this->getData();
}

//////////////////////////////////////////////////////////

template <class NodeType>
class BinaryTree
{
public:
	/** Tree constructor. **/
	BinaryTree();  
	/** Tree destructor. **/
	~BinaryTree();

public:
	/** Adds node. **/
	int AddNode(const NodeType& node);

	/** Adds node. **/
	//TreeNode<NodeType>* DeleteNode(TreeNode<NodeType> *);



private:
	Node<NodeType> *root;

};

