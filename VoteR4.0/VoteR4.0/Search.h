// Signature must be retained. Implementation must be added.

#ifndef SEARCH_H
#define SEARCH_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

// returns FIRST array location matching the given item (based on the comparator)

template <typename T>
long long binarySearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
	long long low = 0;												// straight up binarySearch
	long long mid = 0;
	long long high = array.getSize() - 1;
	while (high >= low) {
		mid = (low + high) / 2;
		if (comparator.compare(item, array.get(mid)) > 0) {
			low = mid + 1;
		}
		else  if (comparator.compare(item, array.get(mid)) < 0) {
			high = mid - 1;
		}
		else {
			long long i = mid;
			do {																		// this part moves index to the left while the index is >=0 and the value there is = to the value at mid
				i--;																	// it stop when it reaches the first occurance of item or actually when it is not equal to item at mid
			} while (i >= 0 && comparator.compare(array.get(i), array.get(mid)) == 0);
			i++;																		// if it stop that mean it is 1 off to the left for the right value, so add 1 back into it
			return i;																	// and return that
		}
	}
	low = (-1 * (low + 1));								// if no matching value found, then return the index where the item is suppose to go
	return low;
}
#endif


