#include "NvraComparator.h"
#include "NvraRecord.h"

NvraComparator::NvraComparator(unsigned int column) {
	this->column = column;
}
int NvraComparator::compare(const NvraRecord& item1, const NvraRecord& item2) const {
	int columnNum;
	if (column < 3) {									// since string and int value are store in different arrays
		columnNum = column;								// we'll need to find out how to know which column correspond with which index of which array
	}
	else if (column == 3) {								// set columnNum equal to the right column 
		columnNum = 0;									// column 3/11/12 is string column which has only 3 indexes value
	}													// everything else is in int column which has 21
	else if (column < 11) {								// validity of column is usually checked before
		columnNum = column - 1;							// this class is called
	}
	else if (column == 11) {
		columnNum = 1;
	}
	else if (column == 12) {
		columnNum = 2;
	}
	else {
		columnNum = column - 3;
	}

	if (column == 3 || column == 11 || column == 12) {						// if it string column then compare by getString
		if (item1.getString(columnNum) > item2.getString(columnNum)) {
			return 1;
		}
		else if (item1.getString(columnNum) < item2.getString(columnNum)) {
			return -1;
		}
		else {
			return 0;
		}
	}
	else {																	// if it int column then compare by getNum
		if (item1.getNum(columnNum) < item2.getNum(columnNum)) {
			return -1;
		}
		else if (item1.getNum(columnNum) > item2.getNum(columnNum)) {
			return 1;
		}
		else {
			return 0;
		}
	}
}