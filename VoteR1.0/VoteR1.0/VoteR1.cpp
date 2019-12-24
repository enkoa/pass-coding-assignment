// GO BEYOND! PLUS ULTRA!
// BAO LE, DATA STRUCTURE

#include "NvraRecord.h"
#include "TemplatedArray.h"
#include "Exceptions.h"
#include "Search.h"
#include "NvraComparator.h"
#include "Sorter.h"
#include "Comparator.h"

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <fstream>
using namespace std;

// function prototype
bool checkID(int recordID, TemplatedArray<NvraRecord>& tempArrays);
bool checkData(string rows, TemplatedArray<NvraRecord>& tempArrays);
void addToList(string rows, TemplatedArray<NvraRecord>& tempArrays);
void outputMethod(TemplatedArray<NvraRecord>& allArrays);
void sortMethod(TemplatedArray<NvraRecord>& allArrays);
void findMethod(TemplatedArray<NvraRecord>& allArrays);
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
bool checkData(string rows, TemplatedArray<NvraRecord>& tempArrays) {
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
					if (!checkID(num, tempArrays)) {	
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
bool checkID(int recordID, TemplatedArray<NvraRecord>& tempArrays) {
	if (recordID < 0) {															// return false if record ID < 0
		cout << "Invalid Data at line " << lineNum << "." << endl;
		return(false);
	}
	else if (tempArrays.getSize() == 0) {										// if array has no data and it not negative
		return(true);															// then return true
	}
	else {
		NvraComparator comparator(0);											// creat dummy NVRA record to store in the record ID
		NvraRecord* dummyRecord;												// also make comparator to compare at column zero (record ID column)
		dummyRecord = new NvraRecord();
		dummyRecord->setNum(recordID, 0);

		correctIndex = binarySearch(*dummyRecord, tempArrays, comparator);		// get an index from binarySearch
		if (correctIndex > 0) {													// if returning index is > 0 then that indicate there is already existing recordID
			cout << "Duplicate record ID " << tempArrays.get(correctIndex).getNum(0) << " at line " << lineNum << "." << endl;
			totalValidData++;													// however, the number of validData still exist, despiting not adding in
			delete dummyRecord;													// free the dummyRecord memory
			return(false);
		}
	}
	return(true);																// happily return true if it pass the test
}

// after passing that checkData the data is finally stored
void addToList(string rows, TemplatedArray<NvraRecord>& tempArrays) {
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

	if (tempArrays.getSize() == 0) {							// if the temporary Array is empty
		tempArrays.add(record);									// then just add it
	}
	else {
		correctIndex = abs(correctIndex) - 1;					// else if it size > 0 then add it to the right spot
		tempArrays.addAt(record, correctIndex);					// add the data to correct index of data
	}
	totalValidData++;											// count total valid record
	delete record;												// free memory
	record = NULL;
}

int main() {
	string firstLine;										
	string rows;
	string fileName;
	string userInput;
	ifstream inFS;

	TemplatedArray<NvraRecord>* allArrays;								// All current array data
	TemplatedArray<NvraRecord>* tempArrays;								// temporary array for the file currently reading

	allArrays = new TemplatedArray<NvraRecord>;							// allocate memory for ALL DATA

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
					tempArrays = new TemplatedArray<NvraRecord>;			// allocate new space if there is a readable file
					if (tempArrays == NULL) {								// check if there is memory
						throw new ExceptionMemoryNotAvailable();
					}
					getline(inFS, firstLine);								// get rid of the first line
					while (getline(inFS, rows)) {
						if (checkData(rows, *tempArrays) == true) {
							try {
								addToList(rows, *tempArrays);					// if pass checkData then add to array
							}
							catch (ExceptionMemoryNotAvailable* ae) {
								delete ae;
							}
						}
						totalLineNum++;										// total data read increases
						lineNum++;											// line for that file increases
					}
					inFS.close();											// close the ifstream
					mergeArray(*allArrays, *tempArrays);
					delete tempArrays;										// delete the data and free the memory from tempArray
				}
				catch (ExceptionMemoryNotAvailable* excptObj) {				
					delete excptObj;
				}
				lineNum = 1;
			}
		}
	} while (fileName.length() != 0);

	if (allArrays->getSize() == 0) {									// exit if no file is inputted
		return 0;
	}

	// DATA MAANIPULATION LOOP
	do {
		cout << "Enter (o)utput, (s)ort, (f)ind, or (q)uit: " << endl;;
		getline(cin, userInput);
		if (userInput == "o") {
			outputMethod(*allArrays);
		}
		else if (userInput == "s") {
			sortMethod(*allArrays);
		}
		else if (userInput == "f") {
			findMethod(*allArrays);
		}
	} while (userInput != "q");

	cout << "Thanks for using VoteR." << endl;

	// system("pause");
	return 0;
}
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
		outFS.open(fileName);												// try to open the file
		if (!outFS.is_open()) {												// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			for (unsigned long i = 0; i < allArrays.getSize(); i++) {		// add in all data
				outFS << allArrays.get(i) << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << allArrays.getSize() << endl;
		}
		else if (outFS.is_open()) {											// if file exis
			for (unsigned long i = 0; i < allArrays.getSize(); i++) {		// then add in data
				outFS << allArrays.get(i) << endl;
			}
			outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << allArrays.getSize() << endl;
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
void findMethod(TemplatedArray<NvraRecord>& allArrays) {
	unsigned int searchField;
	string searchFieldStr;
	cout << "Enter search field (0-23): ";										// ask for column number
	getline(cin, searchFieldStr);
	try {
		searchField = stoi(searchFieldStr);
		NvraComparator comparator(searchField);									// make a comparator for that column
		try {
			if (searchField == 3 || searchField == 11 || searchField == 12) {		// call stringSearch if it is string column
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