// GO BEYOND! PLUS ULTRA!
// https://stackoverflow.com/questions/11842416/function-does-not-change-passed-pointer-c  good stuff on reference of a pointer to change through methods... pointer to pointer etc
// https://stackoverflow.com/questions/17032970/clear-data-inside-text-file-in-c Gr8 website for clear inside text file after opening it
// BAO LE, DATA STRUCTURE

#include "NvraRecord.h"
#include "TemplatedArray.h"
#include "Exceptions.h"
#include "Search.h"
#include "NvraComparator.h"
#include "Sorter.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "Enumerator.h"
#include "HashTable.h"
#include "Hasher.h"
#include "NvraHasher.h"
#include "HashTableEnumerator.h"

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <fstream>
using namespace std;

// function prototype
// voter methods
bool checkID(int recordID, OULinkedList<NvraRecord>& tempList);
bool checkData(string rows, OULinkedList<NvraRecord>& tempList);
void addToList(string rows, OULinkedList<NvraRecord>& tempList);
void dataInput(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays, int check);
void dataManipulation(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays, HashTable<NvraRecord>* hashRecords);

// linked list methods
void purgeList(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays);
void deleteFromList(OULinkedList<NvraRecord>& linkedList, OULinkedList<NvraRecord>& tempList);
void recordList(OULinkedList<NvraRecord>& linkedList);
void mergeLists(OULinkedList<NvraRecord>& linkedList, OULinkedList<NvraRecord>& tempList);
void linkToArray(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays);
void printLinks(OULinkedList<NvraRecord>& linkedList);

// hash table methods
HashTable<NvraRecord>* makeHashTable(OULinkedList<NvraRecord>* linkedList);
void hashMethod(HashTable<NvraRecord>* hashRecord);
void printHash(HashTable<NvraRecord>* hashRecords);
void findRecordID(HashTable<NvraRecord>* hashRecord, unsigned int searchField);

// arrays methods
void outputMethod(TemplatedArray<NvraRecord>& allArrays);
void sortMethod(TemplatedArray<NvraRecord>& allArrays);
void findMethod(TemplatedArray<NvraRecord>& allArrays, HashTable<NvraRecord>* hashRecord);
void printData(TemplatedArray<NvraRecord>& allArrays);
void stringSearches(unsigned int searchField, TemplatedArray<NvraRecord>& allArrays, NvraComparator comparator);
void intSearches(unsigned int searchField, TemplatedArray<NvraRecord>& allArrays, NvraComparator comparator);
void mergeArray(TemplatedArray<NvraRecord>& allArrays, TemplatedArray<NvraRecord>& tempArrays);
void printMatching(unsigned int searchField, long long searchIndex, TemplatedArray<NvraRecord>& allArrays, NvraRecord dummyRecord, NvraComparator comparator);
template<typename T>
long long linearSearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator);

long long lineNum = 1;										// coutning line numbers
long long totalLineNum = 0;									// count total data READ
long long totalValidData = 0;								// since size is private I need my own size outside
unsigned int lastSorted = 0;								// keep track of what was sorted last, inital sorted is at index 0
long long correctIndex = 0;									// correct index placement if binarySearch didn't find something

// check the data
bool checkData(string rows, OULinkedList<NvraRecord>& tempList) {
	stringstream ss;

	// removing all spaces
	for (unsigned int i = 0; i < rows.length(); i++) {
		if (rows.at(i) == ' ') {
			rows = rows.substr(0, i) + rows.substr(i + 1, rows.length());
		}
	}
	// replacing comma with spaces for equal column
	for (unsigned int i = 0; i < rows.length(); i++) {
		if (rows.at(i) == ',') {
			rows.at(i) = ' ';
		}
	}
	ss << rows;															// put the string into string stream

	int num;
	int colN = 0;														// column number
	string tempString;													// temp string to store ss into
	while (!ss.eof() && colN < 24) {
		ss >> tempString;
		if (colN != 3 && colN != 11 && colN != 12) {					// if not the string columns then check validity			
			if (stringstream(tempString) >> num) {						// store string into integers			
				if (colN == 0) {										// if column 0 check prev record ID
					if (!checkID(num, tempList)) {
						return(false);									// return false if recordID is < 0 or already existed
					}
				}
				else if (num < 0) {										// return false if num is < 0
					cout << "Invalid data at line " << lineNum << "." << endl;
					return(false);
				}
			}
			else {														// false if it not int in and int column
				cout << "Invalid data at line " << lineNum << "." << endl;
				return(false);
			}
		}
		colN++;															// column increases to the right
		tempString = "";												// reset temp string
	}
	return(true);
}

// check the ID with the previous IDs and it validity
bool checkID(int recordID, OULinkedList<NvraRecord>& tempList) {
	if (recordID < 0) {															// return false if record ID < 0
		cout << "Invalid Data at line " << lineNum << "." << endl;
		return(false);
	}
	else if (tempList.getSize() == 0) {										// if array has no data and it not negative
		return(true);															// then return true
	}
	else {

		OULinkedListEnumerator<NvraRecord> enumerator = tempList.enumerator();
		int currRecordID = 0;

		while (enumerator.hasNext()) {
			currRecordID = enumerator.next().getNum(0);
			if (currRecordID == recordID) {
				cout << "Duplicate record ID " << currRecordID << " at line " << lineNum << "." << endl;
				totalValidData++;													// however, the number of validData still exist, despiting not adding in
				return(false);
			}
		};
		currRecordID = enumerator.next().getNum(0);								// compare the last element
		if (currRecordID == recordID) {
			cout << "Duplicate record ID " << currRecordID << " at line " << lineNum << "." << endl;
			totalValidData++;													// however, the number of validData still exist, despiting not adding in
			return(false);
		}
	}
	return(true);																// happily return true if it pass the test
}

// after passing that checkData the data is finally stored
void addToList(string rows, OULinkedList<NvraRecord>& tempList) {
	NvraRecord* record = new NvraRecord;					// allocate memory for current data before send

	if (record == NULL) {
		throw new ExceptionMemoryNotAvailable();
	}

	int colN = 0;											// column of the string
	int num = 0;											// to convert string into int

	long scolonPos;											// the position of the commas for substr
	string temp = rows;										// temp string to extract data

	while (rows.find(',') != string::npos) {
		scolonPos = rows.find(',');
		temp = temp.substr(0, scolonPos);						// grab the string that start at 0
																// to the semilcolon
		if (colN != 3 && colN != 11 && colN != 12) {
			num = stoi(temp);									// convert string to int for adding
			record->addNum(num);
		}
		else {
			record->addString(temp);							// add string
		}

		temp = rows.substr(scolonPos + 1, rows.length());		// remove the string that was
		rows = temp;											// added as data from the full 
		colN++;													// string
	}
	num = stoi(temp.substr(0, rows.length()));					// add the last int column
	record->addNum(num);

	try {
		tempList.insert(record);
	}
	catch (ExceptionMemoryNotAvailable* ae) {
		delete ae;
	}

	totalValidData++;											// count total valid record
	delete record;												// free memory
	record = NULL;
}

int main() {
	OULinkedList<NvraRecord>* linkedList;
	NvraComparator* listComparator;
	listComparator = new NvraComparator(0);
	linkedList = new OULinkedList<NvraRecord>(listComparator);

	TemplatedArray<NvraRecord>* allArrays;								// All current array data
	allArrays = new TemplatedArray<NvraRecord>;							// allocate memory for ALL DATA
																		// DATA INPUT LOOP
	dataInput(*linkedList, allArrays, 1);

	if (linkedList->getSize() == 0) {									// exit if no file is inputted
		return 0;
	}

	HashTable<NvraRecord>* hashRecords;									// make hashtable
	hashRecords = makeHashTable(linkedList);

	// DATA MAANIPULATION LOOP
	dataManipulation(*linkedList, allArrays, hashRecords);

	cout << endl << "Thanks for using VoteR." << endl;

	// system("pause");
	return 0;
}
void dataInput(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays, int check) {
	string firstLine;
	string rows;
	string fileName;
	ifstream inFS;
	OULinkedList<NvraRecord>* tempList;								// temporary array for the file currently reading
	NvraComparator* tempComparator;

	tempComparator = new NvraComparator(0);
	tempList = new OULinkedList<NvraRecord>(tempComparator);		// allocate new space if there is a readable file
	if (tempList == NULL) {											// check if there is memory
		throw new ExceptionMemoryNotAvailable();
	}

	do {
		cout << "Enter data file name: ";								// ask file name
		getline(cin, fileName);
		if (fileName.length() > 0) {									// if input file name's length is > 0
			inFS.open(fileName);										// then try to open it
			if (!inFS.is_open()) {
				cout << "File is not available." << endl;				// return false if it can't be open
			}
			else {
				try {
					getline(inFS, firstLine);								// get rid of the first line
					while (getline(inFS, rows)) {
						if (checkData(rows, *tempList) == true) {
							try {
								addToList(rows, *tempList);					// if pass checkData then add to array
							}
							catch (ExceptionMemoryNotAvailable* ae) {
								delete ae;
							}
						}
						totalLineNum++;										// total data read increases
						lineNum++;											// line for that file increases
					}
					if (tempList->getSize() == 0 && check == 1) {
						cout << "No valid records found." << endl;
					}
					inFS.close();											// close the ifstream
				}
				catch (ExceptionMemoryNotAvailable* excptObj) {
					delete excptObj;
				}
				lineNum = 1;
			}
		}
	} while (fileName.length() != 0 && tempList->getSize() == 0);

	if (tempList->getSize() != 0) {
		mergeLists(linkedList, *tempList);
		linkToArray(linkedList, allArrays);
	}
	delete tempList;
}
void dataManipulation(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays, HashTable<NvraRecord>* hashRecords) {
	string userInput;
	do {
		cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, (h)ash table, or (q)uit: ";
		getline(cin, userInput);
		if (userInput == "o") {
			outputMethod(*allArrays);
		}
		else if (userInput == "s") {
			sortMethod(*allArrays);
		}
		else if (userInput == "f") {
			findMethod(*allArrays, hashRecords);
		}
		else if (userInput == "p") {
			purgeList(linkedList, allArrays);
			delete hashRecords;												// we reset hash table whenevr we purge or merge
			hashRecords = makeHashTable(&linkedList);
		}
		else if (userInput == "m") {										// ^ what that statement said
			dataInput(linkedList, allArrays, 2);
			delete hashRecords;
			hashRecords = makeHashTable(&linkedList);
		}
		else if (userInput == "r") {
			recordList(linkedList);
		}
		else if (userInput == "h") {
			hashMethod(hashRecords);
		}
	} while (userInput != "q");
}


// ALL HashTable method

// Make hash table
HashTable<NvraRecord>* makeHashTable(OULinkedList<NvraRecord>* linkedList) {
	NvraComparator* comparator = new NvraComparator(0);
	NvraHasher* hasher = new NvraHasher();

	HashTable<NvraRecord>* hashRecords = new HashTable<NvraRecord>(comparator, hasher, linkedList->getSize());
	if (hashRecords == NULL) {												// check to see if there is memeory
		throw new ExceptionMemoryNotAvailable();
	}
	else {
		OULinkedListEnumerator<NvraRecord> enumerator = linkedList->enumerator();
		if (linkedList->getSize() > 0) {									// otherwise if the linked list has data
			NvraRecord tempRecord;											// then add everything from linked list in
			while (enumerator.hasNext()) {
				tempRecord = enumerator.next();
				hashRecords->insert(&tempRecord);
			}
			tempRecord = enumerator.next();
			hashRecords->insert(&tempRecord);
		}
		return(hashRecords);
	}
}

void hashMethod(HashTable<NvraRecord>* hashRecords) {
	string fileName;
	fstream outFS;
	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);
	if (fileName.length() == 0) {											// if no input then print all the Nvra Records in linked list
		printHash(hashRecords);
		cout << "Base Capacity: " << hashRecords->getBaseCapacity() << "; Total Capacity: " << hashRecords->getTotalCapacity() << "; Load Factor: " << hashRecords->getLoadFactor() << endl;
		cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << hashRecords->getSize() << endl;
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);												// try to open the file
		HashTableEnumerator<NvraRecord> enumerator(hashRecords);

		bool* buckets = new bool[hashRecords->getBaseCapacity()];
		for (unsigned long i = 0; i < hashRecords->getBaseCapacity(); i++) {						// arrays of buckets since all overflow hash key are the same this will help us
			buckets[i] = false;
		}

		if (!outFS.is_open()) {																		// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			if (hashRecords->getSize() > 0) {														// if the hash table has data then
				NvraRecord tempRecord;
				NvraHasher tempHasher;

				unsigned long bucketIndex = -1;														// we use hasher to print bucket
				bool firstPrint = false;															// the first line is tricky
				while (enumerator.hasNext()) {
					tempRecord = enumerator.next();
					bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();
					if (buckets[bucketIndex] == false && firstPrint == false) {						// case of the first bucket to be print
						outFS << bucketIndex << ": " << tempRecord << endl;
						buckets[bucketIndex] = true;
						firstPrint = true;
					}
					else if (buckets[bucketIndex] == false) {										// if the new index is a new bucket then the bucket array will have it as false 
						outFS << endl;
						outFS << bucketIndex << ": " << tempRecord << endl;							// then we can print out a new bucket number
						buckets[bucketIndex] = true;
					}
					else {
						outFS << "OVERFLOW: " << tempRecord << endl;								// otherwise it is an overflow
					}
				}
				tempRecord = enumerator.next();
				bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();
				if (buckets[bucketIndex] == false) {												// same comments as above
					outFS << endl;
					outFS << bucketIndex << ": " << tempRecord << endl;
					buckets[bucketIndex] = true;
				}
				else {
					outFS << "OVERFLOW: " << tempRecord << endl;
				}
				outFS << endl;
			}							
			outFS << "Base Capacity: " << hashRecords->getBaseCapacity() << "; Total Capacity: " << hashRecords->getTotalCapacity() << "; Load Factor: " << hashRecords->getLoadFactor() << endl;
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << hashRecords->getSize() << endl;
			outFS.close();
		}
		else if (outFS.is_open()) {												// if file exist
			if (hashRecords->getSize() > 0) {															// if the linkedlist has data then
				NvraRecord tempRecord;
				NvraHasher tempHasher;

				unsigned long bucketIndex = -1;
				bool firstPrint = false;
				while (enumerator.hasNext()) {
					tempRecord = enumerator.next();
					bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();
					if (buckets[bucketIndex] == false && firstPrint == false) {							// condition for first first printing
						outFS << bucketIndex << ": " << tempRecord << endl;
						buckets[bucketIndex] = true;
						firstPrint = true;
					}
					else if (buckets[bucketIndex] == false) {											// condition for all the bucket after the first
						outFS << endl;
						outFS << bucketIndex << ": " << tempRecord << endl;								// array will tell us whether or not we changed bucket
						buckets[bucketIndex] = true;
					}
					else {
						outFS << "OVERFLOW: " << tempRecord << endl;									// otherwise it is overflow
					}
				}
				tempRecord = enumerator.next();
				bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();
				if (buckets[bucketIndex] == false) {
					outFS << endl;
					outFS << bucketIndex << ": " << tempRecord << endl;
					buckets[bucketIndex] = true;
				}
				else {
					outFS << "OVERFLOW: " << tempRecord << endl;
				}
				outFS << endl;
			}
			outFS << "Base Capacity: " << hashRecords->getBaseCapacity() << "; Total Capacity: " << hashRecords->getTotalCapacity() << "; Load Factor: " << hashRecords->getLoadFactor() << endl;
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << hashRecords->getSize() << endl;
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			delete[] buckets;
			buckets = NULL;
			hashMethod(hashRecords);
		}
		delete[] buckets;													// delete the bucket we made
		buckets = NULL;
	}
}
void printHash(HashTable<NvraRecord>* hashRecords) {
	if (hashRecords->getSize() > 0) {
		HashTableEnumerator<NvraRecord> enumerator(hashRecords);
		NvraHasher tempHasher;													// get temp hasher and temprecord
		NvraRecord tempRecord;
		
		bool* buckets = new bool[hashRecords->getBaseCapacity()];
		for (unsigned long i = 0; i < hashRecords->getBaseCapacity(); i ++) {			// since all overflow have the same hash key this array with same size as
			buckets[i] = false;															// hash table will help us
		}

		unsigned long bucketIndex = -1;
		bool firstPrint = false;
		while (enumerator.hasNext()) {
			tempRecord = enumerator.next();
			bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();	
			if (buckets[bucketIndex] == false && firstPrint == false) {					// if bucket is not printed yet and it is the first one
				cout << bucketIndex << ": " << tempRecord << endl;						// then we print no starting end line
				buckets[bucketIndex] = true;
				firstPrint = true;
			}
			else if (buckets[bucketIndex] == false) {									// after the first statement is passed, whenever we changed bucket
				cout << endl;															// this part will print the endl then start new bucket
				cout << bucketIndex << ": " << tempRecord << endl;
				buckets[bucketIndex] = true;
			}
			else {
				cout << "OVERFLOW: " << tempRecord << endl;								// otherwise it is overflow
			}
		}
		tempRecord = enumerator.next();													// get the last data
		bucketIndex = tempHasher.hash(tempRecord) % hashRecords->getBaseCapacity();
		if (buckets[bucketIndex] == false) {											// this is basically the same as the above statement
			cout << endl;																// since this is the case for the last data
			cout << bucketIndex << ": " << tempRecord << endl;
			buckets[bucketIndex] = true;
		}
		else {
			cout << "OVERFLOW: " << tempRecord << endl;
		}
		cout << endl;

		delete[] buckets;
		buckets = NULL;
	}
}
void findRecordID(HashTable<NvraRecord>* hashRecord, unsigned int searchField) {
	string valueField;
	cout << "Enter non-negative field value: ";										// ask for data
	getline(cin, valueField);
	unsigned int num = stoi(valueField);										// this unsigned wil check if the input is negative
	
	NvraRecord* dummyRecord;
	dummyRecord = new NvraRecord();
	if (dummyRecord == NULL) {													// check to see if there was space
		throw new ExceptionMemoryNotAvailable();
	}
	try {
		dummyRecord->setNum(num, searchField);
		cout << hashRecord->find(dummyRecord) << endl;								// try to print if data exit
		cout << "NVRA records found: 1." << endl;									// exception is thrown if not exit otherwise there is 1 data
		delete dummyRecord;															// delete if found
		return;																		// return
	}
	catch (ExceptionHashTableAccess* ae) {											// catch exceptions
		delete ae;
	}
	catch (ExceptionMemoryNotAvailable* ea) {
		delete ea;
	}
	cout << "NVRA records found: 0." << endl;										// if this point is reach then there is no valid data so there is 0
	delete dummyRecord;																// delete the dummyRecord
}

// ALL linkedlist method, all beside purgeList uses enumerator.


void purgeList(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays) {
	string firstLine;
	string rows;
	string fileName;
	ifstream inFS;
	OULinkedList<NvraRecord>* tempList;									// temporary array for the file currently reading
	NvraComparator* tempComparator;

	tempComparator = new NvraComparator(0);
	tempList = new OULinkedList<NvraRecord>(tempComparator);			// allocate new space if there is a readable file
	if (tempList == NULL) {												// check if there is memory
		throw new ExceptionMemoryNotAvailable();
	}

	do {
		cout << "Enter data file name: ";								// ask file name
		getline(cin, fileName);
		if (fileName.length() > 0) {									// if input file name's length is > 0
			inFS.open(fileName);										// then try to open it
			if (!inFS.is_open()) {
				cout << "File is not available." << endl;				// return false if it can't be open
			}
			else {
				try {
					getline(inFS, firstLine);								// get rid of the first line
					while (getline(inFS, rows)) {
						if (checkData(rows, *tempList) == true) {
							try {
								addToList(rows, *tempList);					// if pass checkData then add to array
							}
							catch (ExceptionMemoryNotAvailable* ae) {
								delete ae;
							}
						}
						totalLineNum++;										// total data read increases
						lineNum++;											// line for that file increases
					}
					if (tempList->getSize() == 0) {							// if the file is empty
						cout << "No valid records found." << endl;
					}
					inFS.close();											// close the ifstream
				}
				catch (ExceptionMemoryNotAvailable* excptObj) {
					delete excptObj;
				}
				lineNum = 1;
			}
		}
	} while (fileName.length() != 0 && tempList->getSize() == 0);

	if (tempList->getSize() != 0) {											// if the file is not empty then purge data
		deleteFromList(linkedList, *tempList);
		linkToArray(linkedList, allArrays);									// re copy to array
	}
	delete tempList;														// free space
}
// finish purging by deleting
void deleteFromList(OULinkedList<NvraRecord>& linkedList, OULinkedList<NvraRecord>& tempList) {
	OULinkedListEnumerator<NvraRecord> enumerator = tempList.enumerator();				// get the enumerator from the temp linked list
	NvraRecord tempRecord;

	if (linkedList.getSize() > 0) {														// only purge data if there is data to purge
		try {
			while (enumerator.hasNext()) {
				tempRecord = enumerator.next();												// get the value from the enumerator
				linkedList.remove(&tempRecord);												// remove it
			}
			tempRecord = enumerator.next();
			linkedList.remove(&tempRecord);													// remove the last element
		}
		catch (ExceptionEnumerationBeyondEnd* ae) {
			delete ae;
		}
	}
}
// print/ record linked list
void recordList(OULinkedList<NvraRecord>& linkedList) {
	string fileName;
	fstream outFS;
	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);
	if (fileName.length() == 0) {											// if no input then print all the Nvra Records in linked list
		printLinks(linkedList);
		cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << linkedList.getSize() << endl;
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);												// try to open the file
		OULinkedListEnumerator<NvraRecord> enumerator = linkedList.enumerator();
		if (!outFS.is_open()) {																		// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			if (linkedList.getSize() > 0) {															// if the linkedlist has data then
				while (enumerator.hasNext()) {														// record in all data
					outFS << enumerator.next() << endl;
				}
				outFS << enumerator.next() << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << linkedList.getSize() << endl;
			outFS.close();
		}
		else if (outFS.is_open()) {												// if file exist
			if (linkedList.getSize() > 0) {										// if the linked list has stuff then
				while (enumerator.hasNext()) {									// add in all data
					outFS << enumerator.next() << endl;
				}
				outFS << enumerator.next() << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << linkedList.getSize() << endl;
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			recordList(linkedList);
		}
	}
}
// merge 2 link list may come back later
void mergeLists(OULinkedList<NvraRecord>& linkedList, OULinkedList<NvraRecord>& tempList) {
	OULinkedListEnumerator<NvraRecord> enumerator = tempList.enumerator();				// make enumerator for the templist
	NvraRecord tempRecord;
	try {
		if (linkedList.getSize() == 0) {													// if the main list is empty then just append that templist in
			while (enumerator.hasNext()) {
				NvraRecord tempRecord = enumerator.next();
				linkedList.append(&tempRecord);
			}
			tempRecord = enumerator.next();
			linkedList.append(&tempRecord);
		}
		else {																				// if it not empty then insert in non duplicates
			while (enumerator.hasNext()) {													// replace all old recordID with new recordID
				NvraRecord tempRecord = enumerator.next();
				if (!linkedList.insert(&tempRecord)) {										// by using insert to check
					linkedList.replace(&tempRecord);
				}
			}
			tempRecord = enumerator.next();
			if (!linkedList.insert(&tempRecord)) {
				linkedList.replace(&tempRecord);
			}
		}
	}
	catch (ExceptionMemoryNotAvailable* ae) {
		delete ae;
	}
}
// print the linked list for record
void printLinks(OULinkedList<NvraRecord>& linkedList) {
	OULinkedListEnumerator<NvraRecord> enumerator = linkedList.enumerator();
	if (linkedList.getSize() > 0) {										// go through the enumerator and print the linked list
		while (enumerator.hasNext()) {
			cout << enumerator.next() << endl;
		}
		cout << enumerator.next() << endl;
	}
}

void linkToArray(OULinkedList<NvraRecord>& linkedList, TemplatedArray<NvraRecord>* allArrays) {

	if (allArrays->getSize() > 0) {																// delete all the element in the array from the back up
		while (allArrays->getSize() > 0) {														// sorry i tried really hard to just make a new array to put in data 
			allArrays->removeAt(allArrays->getSize() - 1);										// then changing the pointer but keep bumping into problems
		}
	}
	if (linkedList.getSize() > 0) {																// copy to array if there is data in the linked list to copy over
		OULinkedListEnumerator<NvraRecord> enumerator = linkedList.enumerator();
		NvraRecord tempRecord;

		while (enumerator.hasNext()) {															// go through the enumerator and add in the data
			tempRecord = enumerator.next();
			allArrays->add(&tempRecord);
		}
		tempRecord = enumerator.next();															// add the last data
		allArrays->add(&tempRecord);
		lastSorted = 0;
	}
}


// UNDER HERE IS METHOD FOR ARRAY


// METHOD FOR OUTPUTING OPTION
void outputMethod(TemplatedArray<NvraRecord>& allArrays) {
	string fileName;
	fstream outFS;
	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);
	if (fileName.length() == 0) {											// if no input then prloall the Nvra Records 
		printData(allArrays);
		cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << allArrays.getSize() << endl;
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);												// try to open the file
		if (!outFS.is_open()) {												// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			for (unsigned long i = 0; i < allArrays.getSize(); i++) {		// add in all data
				outFS << allArrays.get(i) << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << allArrays.getSize() << endl;
			outFS.close();
		}
		else if (outFS.is_open()) {											// if file exis
			for (unsigned long i = 0; i < allArrays.getSize(); i++) {		// then add in data
				outFS << allArrays.get(i) << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << allArrays.getSize() << endl;
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			outputMethod(allArrays);
		}
	}
}
// METHORD FOR SORT OPTION
void sortMethod(TemplatedArray<NvraRecord>& allArrays) {
	string sortFieldStr;
	unsigned int sortField;
	cout << "Enter sort field (0-23): ";										// ask for column number
	getline(cin, sortFieldStr);
	try {
		sortField = stoi(sortFieldStr);
		if (lastSorted != sortField && sortField < 24) {						// if column is a valid column
			NvraComparator comparator(sortField);								// make a comparator for that column
			Sorter<NvraRecord>::sort(allArrays, comparator);					// then sort it
			lastSorted = sortField;
		}
	}
	catch (invalid_argument ae) {}
}
// METHOD FOR FIND OPTION
void findMethod(TemplatedArray<NvraRecord>& allArrays, HashTable<NvraRecord>* hashRecord) {
	unsigned int searchField;
	string searchFieldStr;
	cout << "Enter search field (0-23): ";										// ask for column number
	getline(cin, searchFieldStr);
	try {
		searchField = stoi(searchFieldStr);
		NvraComparator comparator(searchField);									// make a comparator for that column
		try {
			if (searchField == 0) {
				findRecordID(hashRecord, searchField);							// use hash table to find record ID
			}
			else if (searchField == 3 || searchField == 11 || searchField == 12) {		// call stringSearch if it is string column
				stringSearches(searchField, allArrays, comparator);
			}
			else if (searchField < 24) {											// else if the column is valid and is not a string column, then go to intSearch
				intSearches(searchField, allArrays, comparator);
			}
		}
		catch (ExceptionMemoryNotAvailable* ea) {
			delete ea;
		}
	}
	catch (invalid_argument ae) {}
}
// print all data for output method
void printData(TemplatedArray<NvraRecord>& allArrays) {
	for (unsigned long long i = 0; i < allArrays.getSize(); i++) {					// print 
		try {																	// try to get the data
			cout << allArrays.get(i);
			cout << endl;
		}
		catch (ExceptionIndexOutOfRange* excptObj) {							// free space for that new object
			delete excptObj;
		}
	}
}
// stringSearches for findMethod
void stringSearches(unsigned int searchField, TemplatedArray<NvraRecord>& allArrays, NvraComparator comparator) {
	string valueField;
	long long searchIndex;
	cout << "Enter exact text on which to search: ";							// ask for a text
	getline(cin, valueField);
	NvraRecord* dummyRecord;
	dummyRecord = new NvraRecord();												// make a dummy NVRARecord to store in that text
	if (dummyRecord == NULL) {													// check to see if it have space
		throw new ExceptionMemoryNotAvailable();
	}
	if (searchField == 3) {
		dummyRecord->setString(valueField, 0);									// since string array has only 3 column
	}																			// the column number from earlier will decides which index it will be stored in
	else if (searchField == 11) {
		dummyRecord->setString(valueField, 1);
	}
	else {
		dummyRecord->setString(valueField, 2);
	}
	if (searchField == lastSorted) {											// if that column is already sorted 
		searchIndex = binarySearch(*dummyRecord, allArrays, comparator);		// then use binarySearch for that index
		printMatching(searchField, searchIndex, allArrays, *dummyRecord, comparator);
	}																			// after that call this method to look for all matching data
	else {
		searchIndex = linearSearch(*dummyRecord, allArrays, comparator);		// otherwise use linear search
		printMatching(searchField, searchIndex, allArrays, *dummyRecord, comparator);
	}																			// after that call this method to look for all matching data
	delete dummyRecord;															// delete dummy NVRA Record array to free memory
}
// intSearches for findMethod
void intSearches(unsigned int searchField, TemplatedArray<NvraRecord>& allArrays, NvraComparator comparator) {
	string valueField;
	long long searchIndex;
	cout << "Enter non-negative field value: ";										// ask for data
	getline(cin, valueField);
	try {
		unsigned int num = stoi(valueField);										// this unsigned wil check if the input is negative
		NvraRecord* dummyRecord;
		dummyRecord = new NvraRecord();
		if (dummyRecord == NULL) {													// check to see if there was space
			throw new ExceptionMemoryNotAvailable();
		}
		if (searchField < 3) {
			dummyRecord->setNum(num, searchField);									// since int array has only 21 indexes/ column
		}
		else if (searchField < 11) {												// this if statement will determine which indexes the data goes into
			dummyRecord->setNum(num, searchField - 1);
		}
		else if (searchField > 12) {												// more like which is the right indexes
			dummyRecord->setNum(num, searchField - 3);
		}
		if (searchField == lastSorted) {											// if column is sorted then use binarySearch for that index
			searchIndex = binarySearch(*dummyRecord, allArrays, comparator);		// then check all data after that index to see if they match
			printMatching(searchField, searchIndex, allArrays, *dummyRecord, comparator);
		}
		else {
			searchIndex = linearSearch(*dummyRecord, allArrays, comparator);		// otherwise use linearSearch for that index
			printMatching(searchField, searchIndex, allArrays, *dummyRecord, comparator);
		}																			// then look at all the data after that index for matching data
		delete dummyRecord;															// delete dummyNVRA Record to free memory
	}
	catch (invalid_argument ae) {}
}
// Linear search method
template <typename T>
long long linearSearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
	for (unsigned long long i = 0; i < array.getSize(); i++) {
		if (comparator.compare(item, array.get(i)) == 0) {			// straight up linear search
			return i;												// return first occurance of index
		}
	}
	return(-1);														// return -1 if no index found
}
// MERGE temporary array and the main array, allArrays
void mergeArray(TemplatedArray<NvraRecord>& allArrays, TemplatedArray<NvraRecord>& tempArrays) {
	NvraRecord* tempRecords;
	tempRecords = new NvraRecord();											// allocate memory for NVRARecord to copy over
	if (tempRecords == NULL) {												// check if there is space
		throw new ExceptionMemoryNotAvailable();
	}
	if (allArrays.getSize() == 0) {
		for (unsigned long long i = 0; i < tempArrays.getSize(); i++) {			// if allArrays has no data then just add them in
			*tempRecords = tempArrays.get(i);								// copy that data to temp array
			allArrays.add(tempRecords);										// add it in
		}
	}
	else {
		unsigned long long tempArraysIndex = 0;									// if it not empty
		long long insertIndex = 0;
		NvraComparator comparator(0);
		while (tempArraysIndex < tempArrays.getSize()) {					// look at all the data in tempArrays
			insertIndex = binarySearch(tempArrays.get(tempArraysIndex), allArrays, comparator);
			*tempRecords = tempArrays.get(tempArraysIndex);					// copy that data over to tempRecords of NVRA
			if (insertIndex < 0) {											// then binarySearch for record ID in allArrays to see if it already in there
				insertIndex = abs(insertIndex) - 1;							// if there is no similar record ID in allArrays, then add this in
				allArrays.addAt(tempRecords, insertIndex);					// at the index return *-1 then minus 1
			}
			else {
				allArrays.replaceAt(tempRecords, insertIndex);				// otherwise if it there is a recordID duplicate already in allArrays, then overwrite it
			}
			tempArraysIndex++;
		};
	}
	delete tempRecords;															// free up that space that was temporary made
}
// helper method to print all the found data that has the same value
void printMatching(unsigned int searchField, long long searchIndex, TemplatedArray<NvraRecord>& allArrays, NvraRecord dummyRecord, NvraComparator comparator) {
	long long recordsFound = 0;
	if (searchIndex >= 0) {													// if the column is already sorted, and the searchIndex is the first occurance of the found data
		if (searchField == lastSorted) {									// keep moving to the next data until they do not have the same data
			unsigned long long j = searchIndex;
			while (j < allArrays.getSize() && comparator.compare(dummyRecord, allArrays.get(j)) == 0) {
				cout << allArrays.get(j) << endl;							// print the data
				recordsFound++;												// records the number of data found
				j++;
			}
		}
		else {																// if column is not sorted, and searchIndex is the first occurance of the found data
			for (unsigned long long j = (unsigned long long)searchIndex; j < allArrays.getSize(); j++) {
				if (comparator.compare(dummyRecord, allArrays.get(j)) == 0) {
					cout << allArrays.get(j) << endl;						// then continue the linear search from there to find and print all matching data
					recordsFound++;
				}
			}
		}
	}
	cout << "NVRA records found: " << recordsFound << "." << endl;			// print out the records found
}




/*	TEST CODE FOR ADDING INTO HASH TABLE
			OULinkedListEnumerator<NvraRecord> enumerator = linkedList.enumerator();
			NvraRecord* tempRecord;
			while (enumerator.hasNext()) {
				tempRecord = new NvraRecord(enumerator.peek());
				hashRecords->insert(tempRecord);
				enumerator.next();
			}
			tempRecord = new NvraRecord(enumerator.peek());
			hashRecords->insert(tempRecord);
*/