#ifndef AVL_TREE_ENUMERATOR
#define AVL_TREE_ENUMERATOR

#include <stack>
#include "Enumerator.h"
#include "AVLTreeOrder.h"
#include "AVLTree.h"

template <typename T>
class AVLTreeEnumerator : public Enumerator<T>
{
private:
	AVLTreeOrder order;
	std::stack<const AVLTree<T>*> traversalStack;
	void buildTraversalStack(const AVLTree<T>* current);
public:
	AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order);
	virtual ~AVLTreeEnumerator();
	bool hasNext() const;
	T next();						// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peek() const;					// throws ExceptionEnumerationBeyondEnd if no next item is available
	AVLTreeOrder getOrder();		// returns the order of this enumerator (preorder, inorder, or postorder)
};

template <typename T>
void AVLTreeEnumerator<T>::buildTraversalStack(const AVLTree<T>* current) {
	if (current == NULL) {									// case case
		return;
	} 
	else if (order == AVLTreeOrder::preorder) {				// preorder do preorder 
		buildTraversalStack(current->right);
		buildTraversalStack(current->left);
		traversalStack.push(current);
	}
	else if (order == AVLTreeOrder::inorder) {				// inorder do inorder
		buildTraversalStack(current->right);
		traversalStack.push(current);
		buildTraversalStack(current->left);
	}
	else if (order == AVLTreeOrder::postorder) {			// postorder do postorder
		traversalStack.push(current);
		buildTraversalStack(current->right);
		buildTraversalStack(current->left);
	}
	else {
		std::cout << "Trouble encountered in Enumerator buildStack..." << std::endl;
	}
	return;
}
template <typename T>
AVLTreeEnumerator<T>::AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order) {
	this->order = order;
	if (root->getSize() > 0 && (order == AVLTreeOrder::preorder || order == AVLTreeOrder::inorder || order == AVLTreeOrder::postorder) ) {
		buildTraversalStack(root);
	}
	else {
		throw new ExceptionAVLTreeAccess();
	}
}
template <typename T>
AVLTreeEnumerator<T>::~AVLTreeEnumerator() {
	// nothing :)
}
template <typename T>
bool AVLTreeEnumerator<T>::hasNext() const {
	if (!traversalStack.empty()) {
		return(true);
	}
	return(false);
}
template <typename T>
T AVLTreeEnumerator<T>::next() {
	if (!traversalStack.empty()) {
		const AVLTree<T>* currTree = traversalStack.top();				// return top of the stack
		traversalStack.pop();											// then pop the stack
		return(*currTree->data);
	}
	else {
		throw new ExceptionAVLTreeAccess();
	}
}
template <typename T>
T AVLTreeEnumerator<T>::peek() const {
	if (!traversalStack.empty()) {
		const AVLTree<T>* currTree = traversalStack.top();
		return(*currTree->data);										// return the top
	}
	else {
		throw new ExceptionAVLTreeAccess();
	}
}
template <typename T>
AVLTreeOrder AVLTreeEnumerator<T>::getOrder() {
	return(order);
}

#endif // !AVL_TREE_ENUMERATOR