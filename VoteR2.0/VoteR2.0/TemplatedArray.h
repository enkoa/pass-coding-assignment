// These definitions may be used as a basis for your templated array class. However, you are
// not required to use them exactly as they are. You may change details as you see fit.
// Nonetheless, you must retain the signatures of all public member functions (methods) and
// their described functionality, so that they may be used in unit tests.

#ifndef TEMPLATED_ARRAY_H
#define TEMPLATED_ARRAY_H

#include "Exceptions.h"

const unsigned long DEFAULT_ARRAY_CAPACITY = 10;		// capacity used in no arg constructor

template <typename T>
class TemplatedArray {
private:
	unsigned long capacity = DEFAULT_ARRAY_CAPACITY;	// maximum capacity, in items
	unsigned long size = 0;								// actual number of items currently in array
	T* data = NULL;										// pointer to array of any type
	void doubleCapacity();								// method to double array capacity
	void halveCapacity();								// method to halve array capacity 
public:
	TemplatedArray();								    // constructs array with default capacity
	TemplatedArray(unsigned long capacity);			    // constructs array with specified capacity
	virtual ~TemplatedArray();						    // frees array space as object is deleted
	void add(const T* item);						    // adds item, increments size, doubles capacity as necessary
	void addAt(const T* item, unsigned long index);	    // adds item at index, shifts following, doubles capacity as necessary
	void replaceAt(const T* item, unsigned long index); // replaces item at index, otherwise unchanged
	void removeAt(unsigned long index);				    // removes item at index, shifts following back
	T get(unsigned long index) const;				    // returns (copy of) item at index
	unsigned long getCapacity() const;				    // returns the current capacity of the array
	unsigned long getSize() const;					    // returns the current number of items in the array
	T operator[](unsigned long index) const;		    // returns (reference to) item at index
};



// PRIVATE METHOD
template<typename T>
void TemplatedArray<T>::doubleCapacity() {
	T* newData = new T[capacity * 2];									// allocate new memory
	if (data == NULL) {
		throw new ExceptionMemoryNotAvailable();						// throw exception if no memory
	}

	for (unsigned long i = 0; i < capacity; i++) {						// copy them
		newData[i] = data[i];
	}
	capacity = capacity * 2;											// double capacity

	delete[] data;														// delete data from old array
	data = newData;														// pointer to new area
	newData = NULL;														// the newData temp pointer to null
	delete newData;														// delete the temp pointer
}
template<typename T>
void TemplatedArray<T>::halveCapacity() {
	T* newData = new T[capacity / 2];									// allocate  new memory for half the space

	if (data == NULL) {													// throw exception if no memory
		throw new ExceptionMemoryNotAvailable();
	}

	for (unsigned long i = 0; i < size; i++) {							// copy data over
		newData[i] = data[i];
	}
	capacity = capacity / 2;											// capacity is halve

	delete[] data;														// delete old data
	data = newData;														// change pointer to the new space
	newData = NULL;														// set the temp pointer to null
	delete newData;														// delete the temp pointer
}

// PUBLIC METHOD
template<typename T>
TemplatedArray<T>::TemplatedArray() {									// allocate data with initial capacity
	data = new T[capacity];
	if (data == NULL) {
		throw new ExceptionMemoryNotAvailable();
	}
}
template<typename T>
TemplatedArray<T>::TemplatedArray(unsigned long capacity) {				// allocate data with input capacity
	this->capacity = capacity;
	data = new T[this->capacity];
	if (data == NULL) {
		throw new ExceptionMemoryNotAvailable();
	}
}
template<typename T>
TemplatedArray<T>::~TemplatedArray() {									// delete all arrays in data
	delete[] data;
}
template<typename T>
void TemplatedArray<T>::add(const T* record) {
	if ((size + 1) == capacity) {										// if size is same
		doubleCapacity();												// double it
	}
	data[size] = *record;												// record data
	size++;																// size up
}
template<typename T>
T TemplatedArray<T>::get(unsigned long index) const {					// return data 
	if (index >= size) {												// if index is valid
		throw new ExceptionIndexOutOfRange();
	}
	return data[index];
}
template<typename T>
void TemplatedArray<T>::addAt(const T* record, unsigned long index) {
	if (index > size) {
		throw new ExceptionIndexOutOfRange();
	}
	if ((size + 1) == capacity) {										// if list is full before adding
		doubleCapacity();												// double it
	}
	if (index == size) {												// if adding data is at the
		data[size] = *record;											// current empty spot then add it in
		size++;
	}
	else {
		for (unsigned long i = size; i > index; i--) {					// else if it not at current empty spot
			data[i] = data[i - 1];										// move all element to the right 1 indexes
		}
		data[index] = *record;											// add new data at spcifided indexes
		size++;
	}
}
template<typename T>
void TemplatedArray<T>::removeAt(unsigned long index) {
	if (index >= size) {												// error if indexes is out of range
		throw new ExceptionIndexOutOfRange();
	}
	for (unsigned long i = index; i < size; i++) {						// move all element from the right of the index
		data[i] = data[i + 1];											// to the left until it replaces over the
	}																	// index of data wanting to be delete
	size--;
	if ((size + 1) <= (capacity / 2)) {									// if new size can be half
		halveCapacity();												// then half it
	}
}
template<typename T>
void TemplatedArray<T>::replaceAt(const T* record, unsigned long index) {
	if (index >= size) {												// replace data at valid index
		throw new ExceptionIndexOutOfRange();
	}
	data[index] = *record;
}
template<typename T>
unsigned long TemplatedArray<T>::getCapacity() const {
	return capacity;													// return capacity
}
template<typename T>
unsigned long TemplatedArray<T>::getSize() const {
	return size;														// return size
}
template<typename T>
T TemplatedArray<T>::operator[](unsigned long index) const {			// I straight up do not know if this even work
	if (index > size) {													// does the same as get(unsigned long index) method
		throw new ExceptionIndexOutOfRange();
	}
	return data[index];													// return data at index
}
#endif

