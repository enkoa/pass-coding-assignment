// Signature must be retained. Implementation must be added.
#ifndef SORT_H
#define SORT_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

template <typename T>
class Sorter {
private:
	// additional member functions (methods) and variables (fields) may be added
	static void quickSort(TemplatedArray<T>& array, const Comparator<T>& comparator, long left, long right);
	static void swap(TemplatedArray<T>& array, long a, long b);
public:
	static void sort(TemplatedArray<T>& array, const Comparator<T>& comparator);
};

template<typename T>
void Sorter<T>::sort(TemplatedArray<T>& array, const Comparator<T>& comparator) {
	long left = 0;												// sort method prep for whatever sort I want to use
	long right = array.getSize() - 1;
	quickSort(array, comparator, left, right);
}
template<typename T>
void Sorter<T>::quickSort(TemplatedArray<T>& array, const Comparator<T>& comparator, long left, long right) {
	long arraySize = array.getSize();							// straight up quicksort
	if (left <= right) {
		long pivotPoint = (left + right) / 2;
		long i = left;
		long j = right;
		do {
			while (i < arraySize && comparator.compare(array.get(i), array.get(pivotPoint)) <= 0) {
				i += 1;
			};
			while (j >= i && comparator.compare(array.get(j), array.get(pivotPoint)) >= 0) {
				j -= 1;
			};
			if (i < j) {
				swap(array, i, j);
			}
		} while (i < j);
		if (i < pivotPoint) {
			j = i;
		}
		swap(array, pivotPoint, j);
		quickSort(array, comparator, left, j - 1);
		quickSort(array, comparator, j + 1, right);
	}
}
template<typename T>
void Sorter<T>::swap(TemplatedArray<T>& array, long a, long b) {
	T temp = array.get(a);										// swap the 2 value at the 2 index a and b
	T temp2 = array.get(b);
	array.replaceAt(&temp2, a);
	array.replaceAt(&temp, b);
}
#endif
