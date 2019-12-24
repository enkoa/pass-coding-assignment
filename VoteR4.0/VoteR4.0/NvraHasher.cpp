#include "NvraHasher.h"
#include "NvraRecord.h"

unsigned long NvraHasher::hash(const NvraRecord& item) const {
	return (item.getNum(0));											// return recordID 
}