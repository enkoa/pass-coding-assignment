#ifndef AVL_TREE
#define AVL_TREE

#include "Comparator.h"

template <typename T>
class AVLTree {
	template <typename U>
	friend class AVLTreeEnumerator;
private:
	Comparator<T>* comparator = NULL;				// used to determine tree order and item equality
	unsigned long size = 0;							// actual number of items currently in tree
	T* data = NULL;									// pointer to data at this node
	int diff = 0;									// height of right minus height of left
	AVLTree<T>* left = NULL;						// pointer to left subtree
	AVLTree<T>* right = NULL;						// pointer to right subtree
	void zig();										// right rotation
	void zag();										// left rotation
	void zigzag();									// left rotation on left subtree, followed by right rotation
	void zagzig();									// right rotation on right subtree, followed by left rotation
	void rebalance();								// check for and rebalance this node, if needed
public:
	AVLTree(Comparator<T>* comparator);				// creates empty linked tree with comparator
	virtual ~AVLTree();								// deletes all links and their data items

	// if an equivalent item is not already present, insert item in order and return true
	// if an equivalent item is already present, leave tree unchanged and return false
	bool insert(const T* item);

	// if an equivalent item is already present, replace item and return true
	// if an equivalent item is not already present, leave tree unchanged and return false
	bool replace(const T* item);

	// if an equivalent item is already present, remove item and return true
	// if an equivalent item is not already present, leave tree unchanged and return false
	bool remove(const T* item);

	// if an equivalent item is present, return a copy of the item
	// if an equivalent item is not present, throw a new ExceptionLinkedtreeAccess
	T find(const T* item) const;

	unsigned long getSize() const;				// returns the current number of items in the tree
};


template <typename T>
AVLTree<T>::AVLTree(Comparator<T>* comparator) {
	this->comparator = comparator;
	left = NULL;
	right = NULL;
}
template <typename T>
AVLTree<T>::~AVLTree() {
	delete data;								// delete current data
	data = NULL;
	if (left != NULL) {							// delete left subtree
		delete left;
	}
	left = NULL;
	if (right != NULL) {						// delete right subtrww
		delete right;
	}
	right = NULL;
	diff = 0;
}
template <typename T>
bool AVLTree<T>::insert(const T* item) {										// 4 cases
	int olddiff;
	if (data == NULL) {															// if current node is empty then add in
		data = new T(*item);
	} 
	else if (comparator->compare(*item, *data) == 0) {							// if current data equals to item				
		return(false);															
	}
	else if (comparator->compare(*item, *data) < 0) {							// if current data is greater than item
		if (left == NULL) {														// if no left subtree
			AVLTree<T>* newLeft = new AVLTree<T>(comparator);					// make one
			newLeft->insert(item);												// call insert on it
			left = newLeft;														// point left to it
			diff--;																// left got longer
		}
		else {
			olddiff = left->diff;												// else record old diff
			if (!left->insert(item)) {											// call insert on left tree
				return(false);
			}
			if ((olddiff != left->diff) && (left->diff != 0)) {					// if insertion successful then cal new diff
				diff--;
			}
		}
	}
	else if (comparator->compare(*item, *data) > 0) {							// if current data is less than item
		if (right == NULL) {													// if there is no right
			AVLTree<T>* newRight = new AVLTree<T>(comparator);					// make one
			newRight->insert(item);												// call insert on it
			right = newRight;													// point right to it
			diff++;																// right side got longer
		}
		else {
			olddiff = right->diff;												// record old diff
			if (!right->insert(item)) {											// try to insert it
				return(false);
			}
			if ((olddiff != right->diff) && (right->diff != 0)) {				// if insertion successful then calc new diff
				diff++;
			}
		}
	}
	else {
		std::cout << "Uh oh, there is a problem in AVLTree.insert..." << std::endl;
	}

	size++;																		// size increases
	rebalance();																// check for rebalance
	return(true);																// return true if no duplicates
}
template <typename T>
bool AVLTree<T>::replace(const T* item) {
	if (data == NULL) {														// return false if current node is null
		return(false);
	}
	else if (comparator->compare(*item, *data) == 0) {						// if they are same then overwrite pointee
		*data = *item;
		return(true);
	}
	else if (comparator->compare(*item, *data) < 0) {						// else if it is less then call replace on left
		if (!left->replace(item)) {
			return(false);
		}
	}
	else if (comparator->compare(*item, *data) > 0) {						// else if it is larger then call replace on right
		if (!right->replace(item)) {
			return(false);
		}
	}
	else {
		std::cout << "Uh oh... There seems to be trouble in AVLTree replace class..." << std::endl;
	}
	return(true);															// if replace successful return true
}
template <typename T>
bool AVLTree<T>::remove(const T* item) {
	int olddiff;
	if (comparator->compare(*item, *data) == 0) {				// if matching found 3 cases
		if (right == NULL) {										// if no right subtree
			AVLTree<T>* oldleft = left;								// take info of the left tree
			if (oldleft != NULL) {									// left become the current node so copy data over
				*data = *oldleft->data;
				left = oldleft->left;								// copy its left and right too
				right = oldleft->right;

				oldleft->data = NULL;								// then set everything on the old left to null
				oldleft->left = NULL;
				oldleft->right = NULL;

				delete oldleft;										// delete oldleft
				oldleft = NULL;										// set it to null
			}
			else {
				data = NULL;
				left = NULL;										// otherwise if left is null then there is no successor node
				right = NULL;										// set everything to null
			}
			diff = 0;
		}
		else if (left == NULL) {									// otherwise if there is a right then look at the left
			AVLTree<T>* oldright = right;							// since left is null the successor the right node
			if (oldright != NULL) {									// copy all of its data over
				*data = *oldright->data;
				left = oldright->left;
				right = oldright->right;

				oldright->data = NULL;								// then set everything in it to null
				oldright->left = NULL;
				oldright->right = NULL;

				delete oldright;
				oldright = NULL;
			}
			else {
				data = NULL;
				left = NULL;										// probably will never reach but this part means that there is no left or right
				right = NULL;										// so everythign become null
			}
			diff = 0;
		}
		else {														// else we need to find successor node
			AVLTree<T>* succ = right;								// for in order we start on the right
			while (succ->left != NULL) {							// we go down the left tree until we hit the last
				succ = succ->left;
			}
			*data = *succ->data;									// we copy data over

			olddiff = right->diff;									// then we get the right old diff for when we remove successor node
			right->remove(succ->data);								// remove the successor node
			if (right->data == NULL) {								// if the successor node happen to be the right already then
				right->left = NULL;									// set all of its data to null then delete it
				right->right = NULL;
				delete right;
				right = NULL;										// right become null
			}

			if (right == NULL || (right->diff != olddiff && right->diff == 0)) {
				diff--;												// then with the old diff we calc new diff
			}
		}
	}
	else if (comparator->compare(*item, *data) < 0) {				// else if it is less than data
		if (left == NULL) {											// return false if left tree don't exist
			return(false);
		}
		olddiff = left->diff;										// get the old diff
		if (!left->remove(item)) {									// attempt to remove from left
			return(false);											// return false if failed
		}
		if (left->data == NULL) {									// if delete is sucessful and the left node was the deleted node
			left->left = NULL;
			left->right = NULL;										// set all of its data to null then delete it
			delete left;
			left = NULL;											// then point it to null
		}
		if (left == NULL || (left->diff != olddiff && left->diff == 0)) {
			diff++;													// then calc new diff
		}
	}
	else if (comparator->compare(*item, *data) > 0) {				// otherwise if data is greatter than current node's data
		if (right == NULL) {										// return false if there is no right tree
			return(false);
		}
		olddiff = right->diff;										// get olddiff
		if (!right->remove(item)) {									// attempt remove on right
			return(false);											// return false if failed to remove
		}
		if (right->data == NULL) {									// if remove was sucessful and the right node was the one where data was deleted
			right->left = NULL;										// then set all of its data to null then delete it
			right->right = NULL;
			delete right;											// let right point to null
			right = NULL;
		}
		if (right == NULL || (right->diff != olddiff && right->diff == 0)) {
			diff--;													// calc new diff in that case
		}
	}
	else {
		std::cout << "Uh oh... There seems to be trouble in AVLTree remove class..." << std::endl;
	}

	size--;
	rebalance();													// if success then size reduce by 1 and then rebalance is call
	return(true);													// then we return true
}
template <typename T>
T AVLTree<T>::find(const T* item) const {
	if (size == 0) {
		throw new ExceptionAVLTreeAccess();
	}
	else if (comparator->compare(*item, *data) == 0) {						// if matching item is found then return it
		return(*data);
	}	
	else if (comparator->compare(*item, *data) < 0) {					// call find on the left if item < data
		try {
			if (left != NULL) {											// if no left then throw exception
				return(left->find(item));
			}
			else {
				throw new ExceptionAVLTreeAccess();
			}
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
			throw new ExceptionAVLTreeAccess();
		}
	}
	else if (comparator->compare(*item, *data) > 0) {					// call find on right if item > data
		try {
			if (right != NULL) {										// try statement because recursive exceptions
				return(right->find(item));								// if there no right then throw exception
			}
			else {
				throw new ExceptionAVLTreeAccess();
			}
		}
		catch (ExceptionAVLTreeAccess* ae) {							// catch exception
			delete ae;
			throw new ExceptionAVLTreeAccess();
		}
	}
	else {
		throw new ExceptionAVLTreeAccess();
	}
}
template <typename T>
unsigned long AVLTree<T>::getSize() const {
	return(size);
}


// private variable


template <typename T>
void AVLTree<T>::zig() {
	if (left == NULL) {  // no left, no way to zig
		return;
	}
	// keep track of diff of node and left for changes
	int gdiff = diff;
	int pdiff = left->diff;

	// modify the tree
	AVLTree<T>* olnr = left;  // olnr is "old left, new right"
	left = olnr->left;
	olnr->left = olnr->right;
	olnr->right = right;
	right = olnr;

	// note that the modifications kept the node itself in place, so we need to swap its data with its olnr's
	T* tempData = data;
	data = olnr->data;
	olnr->data = tempData;

	// update the diff fields for node and new right
	if (pdiff < 0) {  // if imbalance was outside left heavy (left-left violation)
		diff = gdiff + 2;
		right->diff = gdiff - pdiff + 1;
	}
	else {  // otherwise imbalance was inside left heavy (left-right violation)
		diff = pdiff + 1;
		right->diff = gdiff + 1;
	}
}
template <typename T>
void AVLTree<T>::zag() {
	if (right == NULL) {  // no right, no way to zag
		return;
	}
	// keep track of diff of node and right for changes
	int gdiff = diff;
	int pdiff = right->diff;

	// modify the tree
	AVLTree<T>* ornl = right;  // ornl is "old right, new left"
	right = ornl->right;
	ornl->right = ornl->left;
	ornl->left = left;
	left = ornl;

	// note that the modifications kept the node itself in place, so we need to swap its data with its ornl's
	T* tempData = data;
	data = ornl->data;
	ornl->data = tempData;

	// update the diff fields for node and new left
	if (pdiff > 0) {  // if imbalance was outside right heavy (right-right violation)
		diff = gdiff - 2;
		left->diff = gdiff - pdiff - 1;
	}
	else {  // otherwise imbalance was inside right heavy (right-left violation)
		diff = pdiff - 1;
		left->diff = gdiff - 1;
	}
}
template <typename T>
void AVLTree<T>::zigzag() {
	left->zag();
	zig();
}
template <typename T>
void AVLTree<T>::zagzig() {
	right->zig();
	zag();
}
template <typename T>
void AVLTree<T>::rebalance() {
	if (diff >= -1 && diff <= 1) {							// balanced
		return;
	}
	if (diff < 0 && left->diff <= 0) {						// left heavy
		zig();
	}
	else if (diff < 0 && left->diff > 0) {					// inside left heavy
		zigzag();
	}
	else if (diff > 0 && right->diff < 0) {					// inside right heavy
		zagzig();
	}
	else if (diff > 0 && right->diff >= 0) {				// right heavy
		zag();
	}
	else {
		std::cout << "Uh oh... problem met in AVLTree rebalance..." << std::endl;
	}
}

#endif // !AVL_TREE
