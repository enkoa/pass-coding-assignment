#ifndef NVRA_COMPARATOR_H
#define NVRA_COMPARATOR_H

#include "Comparator.h"
#include "NvraRecord.h"

class NvraComparator : public Comparator<NvraRecord> {
private:
	unsigned int column = 0;
public:
	NvraComparator(unsigned int column);
	int compare(const NvraRecord& item1, const NvraRecord& item2) const;
};

#endif // !NVRA_COMPARATOR_H

