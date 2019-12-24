#ifndef HASH_TABLE
#define HASH_TABLE

#include "Exceptions.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "Hasher.h"

const unsigned int SCHEDULE_SIZE = 25;			// the number of items in the size schedule
const unsigned int SCHEDULE[SCHEDULE_SIZE] = { 1, 2, 5, 11, 23, 53, 107, 223, 449, 907, 1823, 3659, 7309, 14621, 29243, 58511, 117023, 234067, 468157, 936319, 1872667, 3745283, 7490573, 14981147, 29962343 };		// the size schedule (all primes)
const unsigned int DEFAULT_SCHEDULE_INDEX = 3;	// the default position in the size schedule
const unsigned long DEFAULT_BASE_CAPACITY = SCHEDULE[DEFAULT_SCHEDULE_INDEX]; 	// the default size of the array
const float DEFAULT_MAX_LOAD_FACTOR = 0.9f;		// the default load factor used to determine when to increase the table size
const float DEFAULT_MIN_LOAD_FACTOR = 0.4f;		// the default load factor used to determine when to decrease the table size

template <typename T>
class HashTable {
	template <typename U>
	friend class HashTableEnumerator;						// necessary to allow the enumerator to access the table's private data
private:
	Comparator<T>* comparator = NULL;						// used to determine item equality
	Hasher<T>* hasher = NULL;								// used to compute hash value
	unsigned long size = 0;									// actual number of items currently in hash table
	float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR;			// the load factor used to determine when to increase the table size
	float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR;			// the load factor used to determine when to decrease the table size
	unsigned int scheduleIndex = DEFAULT_SCHEDULE_INDEX;	// the index of current location in the size schedule
	unsigned long baseCapacity = DEFAULT_BASE_CAPACITY;		// the size of the array
	unsigned long totalCapacity = baseCapacity;				// the size of the array plus chains of more than one link
	OULinkedList<T>** table = NULL;							// table will be an array of pointers to OULinkedLists of type T
	// you may add additional member variables and functions here to support the operation of your code

	long currScheduleIndex = 0;								// since everything is const we need keep track of the schedule's index for expandin/shrinking
	OULinkedList<T>** initializeTable(unsigned long newBaseCapacity);	// initialize table
	unsigned long findSizeIndex(unsigned long size);		// find the schedule's index for given size
	void expandTable();										// grow table if load factor exceed
	void shrinkTable();										// reduce table if loadfactor is below minimum
	void copyTable(OULinkedList<T>** oldTable, unsigned long oldBaseCapacity);	// copy data from old table to new
	void checkCapacity(bool max);							// check with min/max load factor base on method that calls it
public:
	HashTable(Comparator<T>* comparator, Hasher<T>* hasher);			// creates an empty table of DEFAULT_BASE_CAPACITY
	HashTable(Comparator<T>* comparator, Hasher<T>* hasher,
		// if size given, creates empty table with size from schedule of sufficient capacity (considering maxLoadFactor)
		unsigned long size,
		float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR,
		float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR);
	virtual ~HashTable();

	// if an equivalent item is not already present, insert item at proper location and return true
	// if an equivalent item is already present, leave table unchanged and return false
	bool insert(T* item);

	// if an equivalent item is already present, replace item and return true
	// if an equivalent item is not already present, leave table unchanged and return false
	bool replace(T* item);

	// if an equivalent item is already present, remove item and return true
	// if an equivalent item is not already present, leave table unchanged and return false
	bool remove(T* item);

	// if an equivalent item is present, return a copy of the item
	// if an equivalent item is not present, throw a new ExceptionHashTableAccess
	T find(const T* item) const;

	unsigned long getSize() const;						// returns the current number of items in the table
	unsigned long getBaseCapacity() const;				// returns the current base capacity of the table
	unsigned long getTotalCapacity() const;				// returns the current total capacity of the table
	float getLoadFactor() const;						// returns the current load factor of the table
	unsigned long getBucketNumber(const T* item) const;	// returns the bucket number for an item using its hash function mod array size

};


template <typename T>
void HashTable<T>::expandTable() {
	unsigned long oldBaseCapacity = baseCapacity;						// retain old information for transfer
	OULinkedList<T>** oldTable = table;									// have a new pointer point to old table

	size = 0;
	currScheduleIndex++;												// increase the schedule size to the next odd
	baseCapacity = SCHEDULE[currScheduleIndex];							// baseCapacity becomes the next size
	totalCapacity = baseCapacity;										// total capacity become baseCapacity as everything literraly resets
	table = initializeTable(baseCapacity);								// make a new table under the main table pointer
	copyTable(oldTable, oldBaseCapacity);								// copy data
}
template <typename T>
void HashTable<T>::shrinkTable() {
	unsigned long oldBaseCapacity = baseCapacity;						// retain old information for transfer
	OULinkedList<T>** oldTable = table;									// have a new pointer point to old table

	size = 0;
	currScheduleIndex++;												// decrease the schedule size to the previous odd
	baseCapacity = SCHEDULE[currScheduleIndex];							// baseCapacity becomes the next size
	totalCapacity = baseCapacity;										// total capacity become baseCapacity as everything literraly resets
	table = initializeTable(baseCapacity);								// make a new table under the main table pointer
	copyTable(oldTable, oldBaseCapacity);
}
template <typename T>
void HashTable<T>::copyTable(OULinkedList<T>** oldTable, unsigned long oldBaseCapacity) {
	for (unsigned long i = 0; i < oldBaseCapacity; i++) {										// go through the old table
		if (oldTable[i]->getSize() != 0) {														// if the linkedlist size at that index exist then
			OULinkedListEnumerator<T> enumerator = oldTable[i]->enumerator();					// get the enumerator
			T* item;
			while (enumerator.hasNext()) {														// go through the enumerator
				item = new T(enumerator.peek());														// get the item and do an insert which will add into the new table
				insert(item);
				enumerator.next();
			}
			item = new T(enumerator.peek());													// get the last item
			insert(item);
		}
	}

	delete[] oldTable;																			// free space from the old table
	oldTable = NULL;
}
template <typename T>
void HashTable<T>::checkCapacity(bool max) {
	if (max) {													// if max is true then there was an insert
		if (getLoadFactor() > maxLoadFactor) {					// we check if size is now higher than maxloadfactor
			expandTable();										// if it is then we expand table
		}
	}
	else {														// else then there was a remove
		if (getLoadFactor() < minLoadFactor) {					// we check if it was below minloadfactor
			shrinkTable();										// if it is then we shrink table
		}
	}
}

template <typename T>
unsigned long HashTable<T>::findSizeIndex(unsigned long size) {			// find the the right size for hash table
	for (unsigned long index = 0; index < SCHEDULE_SIZE; index++) {		// if the data size is given
		if (((float)size / (float)SCHEDULE[index]) < maxLoadFactor) {
			return(index);
		}
	}
	return(-1);															// return -1 if there not enough size
}
template <typename T>
OULinkedList<T>** HashTable<T>::initializeTable(unsigned long newBaseCapacity) {
	OULinkedList<T>** newTable = new OULinkedList<T>*[newBaseCapacity];
	if (newTable == NULL) {
		throw new ExceptionMemoryNotAvailable();						// check if there were enough memory
	}
	for (unsigned long i = 0; i < newBaseCapacity; i++) {				// create all linkedlists so they have size =0
		newTable[i] = new OULinkedList<T>(comparator);
	}
	return(newTable);
}
template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher) {		// constructor
	this->comparator = comparator;
	this->hasher = hasher;
	baseCapacity = DEFAULT_BASE_CAPACITY;									// default constructor so we have default base capacity
	totalCapacity = baseCapacity;											// total capacity is the same initially
	table = initializeTable(baseCapacity);									// make table
	currScheduleIndex = DEFAULT_SCHEDULE_INDEX;								// the currScedule exist to keep track of current size's index
}
template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher, unsigned long size, float maxLoadFactor, float minLoadFactor) {
	this->comparator = comparator;
	this->hasher = hasher;
	this->maxLoadFactor = maxLoadFactor;
	this->minLoadFactor = minLoadFactor;
	currScheduleIndex = findSizeIndex(size);								// since this has given size we need to find the right schedule size for the table
	baseCapacity = SCHEDULE[currScheduleIndex];								// same as default constructor
	totalCapacity = baseCapacity;
	table = initializeTable(baseCapacity);									// make table
}
template <typename T>
HashTable<T>::~HashTable() {
	delete[] table;															// delete all the linkedlists in array
}
template <typename T>
bool HashTable<T>::insert(T* item) {
	unsigned long bucket = getBucketNumber(item);
	if (table[bucket]->getSize() == 0) {									// if the linked list is empty then add in
		if (table[bucket]->insert(item)) {
			size++;															// increase the size then send true to checkCapacity as 
			checkCapacity(true);											// we want to check with maxloadfactor
			return(true);
		}
	}
	else {
		if (table[bucket]->insert(item)) {									// otherwise it is overflow
			size++;															// then we increase size and totalCapacity
			totalCapacity++;
			checkCapacity(true);											// same reason as above
			return(true);
		}
	}
	return(false);															// return false if item exist
}
template <typename T>
bool HashTable<T>::replace(T* item) {										
	unsigned long bucket = getBucketNumber(item);
	if (table[bucket]->getSize() != 0) {									// if the linkedlist has data in it
		if (table[bucket]->replace(item)) {									// call replaceo n linked list
			return(true);
		}
	}
	return(false);															// return false if no data exist to replace
}
template <typename T>
bool HashTable<T>::remove(T* item) {
	unsigned long bucket = getBucketNumber(item);					
	if (table[bucket]->getSize() != 0) {									// if linkedlist at the bucket of that item have data
		if (table[bucket]->remove(item)) {									// call remove from linked list
			size--;															// reduce size then call checkCapacity with false
			checkCapacity(false);											// as we want to check with minloadfactor
			return(true);
		}
	}
	return(false);															// return false if there is no item in the table
}
template <typename T>
T HashTable<T>::find(const T* item) const {								
	unsigned long bucket = getBucketNumber(item);
	try {																	// go straight and use the linkedlist find method
		return(table[bucket]->find(item));
	}
	catch (ExceptionLinkedListAccess* ae) {									// if item is not there an exception will be thrown
		delete ae;
		throw new ExceptionHashTableAccess();								// we catch it here
	}
}
template <typename T>
unsigned long HashTable<T>::getSize() const {
	return(size);																// return size
}
template <typename T>
unsigned long HashTable<T>::getBaseCapacity() const {							// return base capacity
	return(baseCapacity);
}
template <typename T>
unsigned long HashTable<T>::getTotalCapacity() const {							// return total capacity
	return(totalCapacity);
}
template <typename T>
float HashTable<T>::getLoadFactor() const {										// calc loadfactor then return it
	float loadFactor = ((float)size / (float)totalCapacity);
	return(loadFactor);
}
template <typename T>
unsigned long HashTable<T>::getBucketNumber(const T* item) const {				// use the hasher to hash the item and then mod with table size
	unsigned long bucketNum = hasher->hash(*item) % baseCapacity;				// to find the bucket number
	return(bucketNum);
}
#endif // !HASH_TABLE
