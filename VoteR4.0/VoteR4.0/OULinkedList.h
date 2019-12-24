#ifndef OU_LINKED_LIST
#define OU_LINKED_LIST

#include "OULink.h"
#include "Comparator.h"
#include "OULinkedListEnumerator.h"

// OULinkedList stands for Ordered, Unique Linked List. It is a linked list that is always maintained in
// order (based on the comparator provided to it when the list is created) and that only contains unique
// items (that is, duplicates are not allowed)
template <typename T>
class OULinkedList {
	//template <typename T>
	//friend class OULinkedListEnumerator;
private:
	Comparator<T>* comparator = NULL;				// used to determine list order and item equality
	unsigned long size = 0;							// actual number of items currently in list
	OULink<T>* first = NULL;						// pointer to first link in list
	OULink<T>* last = NULL;							// pointer to last link in list
public:
	OULinkedList(Comparator<T>* comparator);		// creates empty linked list with comparator
	virtual ~OULinkedList();						// deletes all links and their data items

	// if an equivalent item is not already present, insert item in order and return true
	// if an equivalent item is already present, leave list unchanged and return false
	bool insert(const T* item);

	// if item is greater than item at last, append item at end and return true
	// if item is less than or equal to item at last, leave list unchanged and return false
	bool append(const T* item);

	// if an equivalent item is already present, replace item and return true
	// if an equivalent item is not already present, leave list unchanged and return false
	bool replace(const T* item);

	// if an equivalent item is already present, remove item and return true
	// if an equivalent item is not already present, leave list unchanged and return false
	bool remove(const T* item);

	// if any items are present, return a copy of the first item
	// if no items are present, throw new ExceptionLinkedListAccess
	T get() const;

	// if an equivalent item is present, return a copy of the first such item
	// if an equivalent item is not present, throw a new ExceptionLinkedListAccess
	T find(const T* item) const;

	unsigned long getSize() const;					// returns the current number of items in the list

	OULinkedListEnumerator<T> enumerator() const;	// create an enumerator for this linked list
};



template <typename T>
OULinkedList<T>::OULinkedList(Comparator<T>* comparator) {								// take in a comparator for the list
	this->comparator = comparator;
}
template <typename T>
OULinkedList<T>::~OULinkedList() {
	delete first;																		// delete the first which will recursively delete all others
	first = NULL;																		// then set first and last pointer to null
	last = NULL;
}
template <typename T>
bool OULinkedList<T>::insert(const T* item) {
	if (size == 0) {																	// if size is 0 then just add it in
		OULink<T>* newLink;
		newLink = new OULink<T>(item);
		if (newLink == NULL) {
			throw new ExceptionMemoryNotAvailable();
		}
		first = newLink;
		last = newLink;
		size++;
	}
	else {
		OULink<T>* curr = first;
		OULink<T>* prev = first;
		while (curr != NULL) {																// else we go through the whole list
			if (comparator->compare(*item, *curr->data) == 0) {								// until we either hit same data
				return(false);
			}
			else if (comparator->compare(*item, *curr->data) < 0) {							// or we hit a data that is greater than item
				break;																		// then we exit
			}
			prev = curr;
			curr = curr->next;
		};
		OULink<T>* newLink;																	// if we made this far then there is duplicates
		newLink = new OULink<T>(item);
		if (newLink == NULL) {
			throw new ExceptionMemoryNotAvailable();
		}
		if (curr == NULL) {																	// curr is null then it mean prev is the last 
			prev->next = newLink;															// so we have just append
			last = newLink;
		}
		else if (comparator->compare(*prev->data, *curr->data) == 0) {						// if prev's data and curr's data is the same then it mean it is the first
			newLink->next = first;															// pointer so we have the newLink set a the front by setting its next to first
			first = newLink;																// then setting first to newlink
		}
		else {
			prev->next = newLink;															// otherwise we have the prev's next point to newLink
			newLink->next = curr;															// and then have newLink's next point to curr
		}
		size++;
	}

	return(true);
}
template <typename T>
bool OULinkedList<T>::append(const T* item) {
	if (size == 0) {																// if list has no element then just add it in
		OULink<T>* newLink;
		newLink = new OULink<T>(item);
		if (newLink == NULL) {
			throw new ExceptionMemoryNotAvailable();
		}
		first = newLink;
		last = newLink;
		size++;
		return(true);
	}
	else if (comparator->compare(*item, *last->data) > 0) {							// else
		OULink<T>* newLink;															// we compare with the last element
		newLink = new OULink<T>(item);												// if it is greater then we add it in
		if (newLink == NULL) {
			throw new ExceptionMemoryNotAvailable();
		}
		last->next = newLink;														// then return true
		last = newLink;
		size++;
		return(true);
	}
	return(false);																	// otherwise we return false
}
template <typename T>
bool OULinkedList<T>::replace(const T* item) {
	if (size > 0) {
		OULink<T>* curr = first;
		OULink<T>* prev = first;
		while (curr != NULL) {
			if (comparator->compare(*item, *curr->data) == 0) {								// if there is matching data
				OULink<T>* newLink;															// create a new link for item
				newLink = new OULink<T>(item);
				if (newLink == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}
				if (comparator->compare(*curr->data, *first->data) == 0) {					// if it is replacing the first node
					newLink->next = curr->next;												// the newLink next is the curr's next
					first = newLink;														// point th first pointer to newlink
					curr->next = NULL;														// the curr's next set to null so we can delete
					delete curr;
				}
				else if (comparator->compare(*curr->data, *last->data) == 0) {				// if it is replacing the last node
					prev->next = newLink;													// have the prev's next point point to the newLink
					last = newLink;															// last pointer point to newLink
					curr->next = NULL;														// the curr's next set to null so we can delete
					delete curr;
				}
				else {
					prev->next = newLink;													// if it replacing anywhere else then
					newLink->next = curr->next;												// prev's next is the newLink
					curr->next = NULL;														// while the newLink's next is the curr's next
					delete curr;															// set curr's next to null so we can delete
				}
				return(true);
			}
			prev = curr;
			curr = curr->next;
		};
	}
	return(false);
}
template <typename T>
bool OULinkedList<T>::remove(const T* item) {
	if (size > 0) {
		OULink<T>* curr = first;
		OULink<T>* prev = first;
		while (curr != NULL) {
			if (comparator->compare(*item, *curr->data) == 0) {								// comparing elements
				if (size == 1) {
					curr->next = NULL;
					first = NULL;															// delete and point first and last to null
					last = NULL;															// if the there only 1 data
					delete curr;
				}
				else if (comparator->compare(*curr->data, *first->data) == 0) {				// if it is the first
					first = curr->next;														// point first to the next one then delete curr
					curr->next = NULL;
					delete curr;
				}
				else if (comparator->compare(*curr->data, *last->data) == 0) {				// if it is the last
					last = prev;															// point last to the prev one then delete curr
					prev->next = NULL;
					delete curr;
				}
				else {
					prev->next = curr->next;												// otherwise the prev next pointer points to the one after curr
					curr->next = NULL;
					delete curr;
				}
				size--;																		// decrease size if there is matching record
				return(true);
			}
			prev = curr;																	// prev point to the link that was just finish comapring
			curr = curr->next;																// moving to the next link
		};
	}
	return(false);
}
template <typename T>
T OULinkedList<T>::get() const {
	if (first != NULL) {																	// if the first link is not null then return its data
		return (*first->data);
	}
	else {
		throw new ExceptionLinkedListAccess();
	}
}
template <typename T>
T OULinkedList<T>::find(const T* item) const {
	OULink<T>* curr = first;
	while (curr != NULL) {																// as long curr is not null 
		if (comparator->compare(*item, *curr->data) == 0) {								// compare with each data until matching data is found
			return (*curr->data);
		}
		curr = curr->next;
	};
	throw new ExceptionLinkedListAccess();												// otherwise throw linkedlist access
}
template <typename T>
unsigned long OULinkedList<T>::getSize() const {										// return size
	return(size);
}
template <typename T>
OULinkedListEnumerator<T> OULinkedList<T>::enumerator() const {							// return enumerator for this list
	OULinkedListEnumerator<T> enumerator(first);
	return(enumerator);
}

#endif // !OU_LINKED_LIST

