#ifndef HASH_TABLE_ENUMERATOR
#define HASH_TABLE_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "HashTable.h"

template <typename T>
class HashTableEnumerator : public Enumerator<T>
{
private:
	unsigned long bucket = 0;								// the current bucket during the enumeration process
	OULinkedListEnumerator<T>* chainEnumerator = NULL;		// used to move through the linked list of the current bucket
	HashTable<T>* hashTable = NULL;							// pointer to the HashTable being enumerated
	// you may add additional member variables and functions here to support the operation of your code
	long findNextFillBucket(long nextPos) const;			// method to find next available bucket
public:
	HashTableEnumerator(HashTable<T>* hashTable);			// constructor needs a pointer to the HashTable to be enumerated
	virtual ~HashTableEnumerator();
	bool hasNext() const;									// true if there are elements that have not yet been returned via next()
	T next();												// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peek() const;											// throws ExceptionEnumerationBeyondEnd if no next item is available
};


template <typename T>
long HashTableEnumerator<T>::findNextFillBucket(long nextPos) const {
	long capacity = hashTable->baseCapacity;								// nexpos determine where after the bucket we are searching
	for (long i = bucket + nextPos; i < capacity; i++) {					// go through 
		if (hashTable->table[i]->getSize() != 0) {							// if found 1 linkedlist that has data then return the bucket to that linked list
			return(i);
		}
	}
	return(-1);																// otherwise we return -1
}
template <typename T>
HashTableEnumerator<T>::HashTableEnumerator(HashTable<T>* hashTable) {
	if (hashTable->size != 0) {
		this->hashTable = hashTable;																// copy hash table over
		bucket = findNextFillBucket(0);
		chainEnumerator = new OULinkedListEnumerator<T>(hashTable->table[bucket]->enumerator());	// creat enumarator
	}
	else {
		throw new ExceptionHashTableAccess();														// throw this if the hashtable is 0
	}
}
template <typename T>
HashTableEnumerator<T>::~HashTableEnumerator() {		// no need for this as my code doesn't call new when making linked list enumerator
	//delete chainEnumerator;
}
template <typename T>
bool HashTableEnumerator<T>::hasNext() const {
	if (chainEnumerator->hasNext()) {					// if the linked list enumerator has a next item then return true
		return(true);
	}
	if (findNextFillBucket(1) >= 0){					// otherwise all bucket starting at bucket + 1, hence the (1) and if there is data
		return(true);									// that method will return its index
	}
	return(false);										// otherwise no more item
}
template <typename T>
T HashTableEnumerator<T>::next() {
	try {
		if (chainEnumerator->hasNext()) {				// if there is a next then return data and move pointer to that next
			return(chainEnumerator->next());
		}
		else if (!chainEnumerator->hasNext()) {			// however if there is no next then we reached the end of that linked list
			T returnData = chainEnumerator->next();		// get the last item
			delete chainEnumerator;						// delete that enumerator
			long nextBucket = findNextFillBucket(1);	// find the next available bucket using that method
			if (nextBucket >= 0) {						// if that method return number >= 0 then that bucket have data
				bucket = nextBucket;					// if that is true then we make that the new bucket and create an enumerator for that
				chainEnumerator = new OULinkedListEnumerator<T>(hashTable->table[bucket]->enumerator());
			}
			return(returnData);							// retur nthat copy data
		}
	}
	catch (ExceptionEnumerationBeyondEnd* ae) {
		delete ae;										// catch exception if attemping next on no item
		throw new ExceptionEnumerationBeyondEnd();
	}
	throw new ExceptionEnumerationBeyondEnd();
}
template <typename T>
T HashTableEnumerator<T>::peek() const {			// if chainenumerator is not false
	try {
		if (chainEnumerator != NULL) {				// then call its peek method for data
			return(chainEnumerator->peek());
		}
	}
	catch (ExceptionEnumerationBeyondEnd* ae) {		// if there is an error for some reason we will catch the exception
		delete ae;
		throw new ExceptionEnumerationBeyondEnd();
	}
	throw new ExceptionEnumerationBeyondEnd();
}
#endif // !HASH_TABLE_ENUMERATOR