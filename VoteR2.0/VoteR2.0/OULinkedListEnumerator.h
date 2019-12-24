#ifndef OU_LINKED_LIST_ENUMERATOR
#define OU_LINKED_LIST_ENUMERATOR

#include "Enumerator.h"
#include "OULink.h"

template <typename T>
class OULinkedListEnumerator : public Enumerator<T>
{
private:
	OULink<T>* current;
public:
	OULinkedListEnumerator(OULink<T>* first);
	bool hasNext() const;
	T next();
	T peek() const;
};



template <typename T>
OULinkedListEnumerator<T>::OULinkedListEnumerator(OULink<T>* first) {
	current = first;															// point curren to the first data
}
template <typename T>
bool OULinkedListEnumerator<T>::hasNext() const{
	if (current == NULL) {														// return false if current is null
		return(false);
	}
	if (current->next == NULL) {												// then return false if current is null
		return(false);
	}
	return(true);
}
template <typename T>
T OULinkedListEnumerator<T>::next() {
	if (current != NULL) {														// if current is not null
		OULink<T>* returnLink = current;										// copy the data for current
		current = current->next;												// move link point to next data
		return(*returnLink->data);												// return current data
	}
	throw new ExceptionEnumerationBeyondEnd();
}
template <typename T>
T OULinkedListEnumerator<T>::peek() const {
	if (current != NULL) {														// if current is not null then return data
		return(*current->data);
	}
	throw new ExceptionEnumerationBeyondEnd();
}
#endif // !OU_LINKED_LIST_ENUMERATOR
