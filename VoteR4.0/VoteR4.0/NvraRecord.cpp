#include "NvraRecord.h"
#include <string>
#include <ostream>
using namespace std;

void NvraRecord::addNum(unsigned int num) {						// add num then increase index
	if (numCtr > 20) {
		throw new ExceptionIndexOutOfRange();
	}
	nums[numCtr] = num;
	numCtr++;
}
void NvraRecord::addString(string string) {						// add string then increase index
	if (strCtr > 2) {
		throw new ExceptionIndexOutOfRange();
	}
	strings[strCtr] = string;
	strCtr++;
}
unsigned int NvraRecord::getNum(unsigned int index) const {		// get num at an index
	if (index > 20) {
		throw new ExceptionIndexOutOfRange();
	}
	return (nums[index]);
}
string NvraRecord::getString(unsigned int index) const {		// get string at an index
	if (index > 2) {
		throw new ExceptionIndexOutOfRange();
	}
	return (strings[index]);
}
void NvraRecord::setNum(unsigned int num, unsigned int index) {	// set num at an index
	if (index > 20) {
		throw new ExceptionIndexOutOfRange();
	}
	nums[index] = num;
}
void NvraRecord::setString(std::string string, unsigned int index) {	// set string at an index
	if (index > 2) {
		throw new ExceptionIndexOutOfRange();
	}
	strings[index] = string;
}
std::ostream& operator<<(std::ostream& os, const NvraRecord& record) {
	int intCol = 0, strCol = 0;
	int col;
	for (col = 0; col < 23; col++) {
		if (col == 3 || col == 11 || col == 12) {
			os << record.strings[strCol] << ";";		// print string then increment str index
			strCol++;
		}
		else {
			os << record.nums[intCol] << ";";			// print num then increment int index
			intCol++;
		}
	}
	os << record.nums[intCol];							// print last num
	return (os);
}
