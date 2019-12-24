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

#include "AVLTree.h"
#include "AVLTreeOrder.h"
#include "AVLTreeEnumerator.h"

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <fstream>
using namespace std;

// function prototype
// voter methods
bool checkData(string rows);
void addToList(string rows, AVLTree<NvraRecord>* tempTree);
AVLTree<NvraRecord>* dataInput(AVLTree<NvraRecord>* treeRecords, int check);
void dataManipulation(AVLTree<NvraRecord>* tempTree, TemplatedArray<NvraRecord>* allArrays, HashTable<NvraRecord>* hashRecords);

// hash table methods
HashTable<NvraRecord>* makeHashTable(OULinkedList<NvraRecord>* linkedList);
void hashMethod(HashTable<NvraRecord>* hashRecord);
void printHash(HashTable<NvraRecord>* hashRecords);
void findRecordID(HashTable<NvraRecord>* hashRecord, unsigned int searchField);

// AVL tree methods
void printPreOrder(AVLTree<NvraRecord>* treeRecords);
void printInOrder(AVLTree<NvraRecord>* treeRecords);
void printPostOrder(AVLTree<NvraRecord>* treeRecords);
AVLTree<NvraRecord>* mergeTree(AVLTree<NvraRecord>* treeRecords, AVLTree<NvraRecord>* tempTree);
AVLTree<NvraRecord>* purgeTree(AVLTree<NvraRecord>* treeRecords);
TemplatedArray<NvraRecord>* treeToArray(AVLTree<NvraRecord>* treeRecords);
HashTable<NvraRecord>* makeHashTable(AVLTree<NvraRecord>* treeRecords);

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

// check if there is invalid data
bool checkData(string rows) {
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
				if (num < 0) {										// return false if num is < 0
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

// after passing that checkData the data is finally stored
void addToList(string rows, AVLTree<NvraRecord>* tempTree) {
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

	// addToList DEALS WITH DUPLICATES DATA HERE
	try {
		if (!tempTree->insert(record)) {
			cout << "Duplicate record ID " << record->getNum(0) << " at line " << lineNum << "." << endl;
		}
	}
	catch (ExceptionMemoryNotAvailable* ae) {
		delete ae;
	}

	totalValidData++;

	delete record;												// free memory
	record = NULL;
}

int main() {
	NvraComparator* listComparator;
	listComparator = new NvraComparator(0);
	AVLTree<NvraRecord>* treeRecords;
	treeRecords = new AVLTree<NvraRecord>(listComparator);
					
	// This 1 means that this is the first data input, so there is only 1 file
	
	treeRecords = dataInput(treeRecords, 1);							// get first data

	if (treeRecords->getSize() == 0) {									// exit if no file is inputted
		return 0;
	}

	TemplatedArray<NvraRecord>* allArrays;								// All current array data
	HashTable<NvraRecord>* hashRecords;									// make hashtable
	hashRecords = makeHashTable(treeRecords);							// make array
	allArrays = treeToArray(treeRecords);

	// DATA MAANIPULATION LOOP
	dataManipulation(treeRecords, allArrays, hashRecords);

	cout << endl << "Thanks for using VoteR." << endl;

	// system("pause");
	return 0;
}
AVLTree<NvraRecord>* dataInput(AVLTree<NvraRecord>* treeRecords, int check) {
	string firstLine;
	string rows;
	string fileName;
	ifstream inFS;
	AVLTree<NvraRecord>* tempTree;										// temporary tree for the file currently reading
	NvraComparator* tempComparator;

	tempComparator = new NvraComparator(0);
	tempTree = new AVLTree<NvraRecord>(tempComparator);					// allocate new space if there is a readable file
	if (tempTree == NULL) {												// check if there is memory
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
						if (checkData(rows) == true) {			// this will check for invalid data
							try {
								addToList(rows, tempTree);					// will attempt to add to the list THIS METHOD DEAL
							}												// WITH DUPLICATES DATA NOW
							catch (ExceptionMemoryNotAvailable* ae) {
								delete ae;
							}
						}
						totalLineNum++;										// total data read increases
						lineNum++;											// line for that file increases
					}
					if (tempTree->getSize() == 0 && check == 1) {
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
	} while (fileName.length() != 0 && tempTree->getSize() == 0);


	if (tempTree->getSize() != 0 && treeRecords->getSize() == 0) {					// if the main tree has no data then the tempTree is the main train
		delete treeRecords;
		return(tempTree);
	}
	else if (check == 1) {															// initially, the tempTree is the new tree
		delete treeRecords;
		return(tempTree);
	}
	else if (tempTree->getSize() != 0 && treeRecords->getSize() != 0) {				// if both file size has data then we can merge
		treeRecords = mergeTree(treeRecords, tempTree);
		delete tempTree;
		return(treeRecords);
	}
	else {
		return(treeRecords);														// otherwise this would mean tempTree has no data in this case we just return the tree we already have
	}
}
void dataManipulation(AVLTree<NvraRecord>* treeRecords, TemplatedArray<NvraRecord>* allArrays, HashTable<NvraRecord>* hashRecords) {
	string userInput;
	do {
		cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (h)ash table, (pre)order, (in)order, (post)order or (q)uit: ";
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
			treeRecords = purgeTree(treeRecords);
			cout << "fin purge";
			delete hashRecords;
			delete allArrays;
			hashRecords = makeHashTable(treeRecords);
			allArrays = treeToArray(treeRecords);
		}
		else if (userInput == "m") {										
			treeRecords = dataInput(treeRecords, 2);
			delete hashRecords;
			delete allArrays;
			hashRecords = makeHashTable(treeRecords);
			allArrays = treeToArray(treeRecords);
		}
		else if (userInput == "h") {
			hashMethod(hashRecords);
		}
		else if (userInput == "pre") {
			printPreOrder(treeRecords);
		}
		else if (userInput == "post") {
			printPostOrder(treeRecords);
		}
		else if (userInput == "in") {
			printInOrder(treeRecords);
		}
	} while (userInput != "q");
}


// ALL AVL Tree METHOD


// make hash table with avl tree
HashTable<NvraRecord>* makeHashTable(AVLTree<NvraRecord>* treeRecords) {
	NvraComparator* comparator = new NvraComparator(0);
	NvraHasher* hasher = new NvraHasher();

	HashTable<NvraRecord>* hashRecords = new HashTable<NvraRecord>(comparator, hasher, treeRecords->getSize());

	if (hashRecords == NULL) {												// check to see if there is memeory
		throw new ExceptionMemoryNotAvailable();
	}
	else {
		try {
			AVLTreeOrder order = AVLTreeOrder::inorder;
			AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);

			NvraRecord data;
			while (enumerator->hasNext()) {						// copy the data from tree to hashtable
				data = enumerator->peek();
				hashRecords->insert(&data);
				enumerator->next();
			}
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
		}

		return(hashRecords);
	}
}
TemplatedArray<NvraRecord>* treeToArray(AVLTree<NvraRecord>* treeRecords) {
	TemplatedArray<NvraRecord>* allArrays;
	allArrays = new TemplatedArray<NvraRecord>;

	if (allArrays == NULL) {												// check to see if there is memeory
		throw new ExceptionMemoryNotAvailable();
	}
	else {
		try {
			AVLTreeOrder order = AVLTreeOrder::inorder;
			AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);

			NvraRecord data;
			while (enumerator->hasNext()) {
				data = enumerator->peek();									// copy data from trees to array
				allArrays->add(&data);
				enumerator->next();
			}
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
		}
		return(allArrays);
	}
}
void printPreOrder(AVLTree<NvraRecord>* treeRecords) {
	string fileName;
	fstream outFS;

	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);

	// line with outFS literally mean writing the file out

	if (fileName.length() == 0) {											// if no input then print all the Nvra Records in trees in the requested order
		try {
			AVLTreeOrder order = AVLTreeOrder::preorder;
			AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
			if (enumerator == NULL) {
				throw new ExceptionMemoryNotAvailable();
			}

			while (enumerator->hasNext()) {
				cout << enumerator->next() << endl;
			}
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			delete enumerator;
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
		}
		catch (ExceptionMemoryNotAvailable* ea) {
			delete ea;
		}
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);						// try to open the file
		if (!outFS.is_open()) {												// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			try {
				AVLTreeOrder order = AVLTreeOrder::preorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}

				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else if (outFS.is_open()) {											// if file exis
			try {
				AVLTreeOrder order = AVLTreeOrder::preorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}

				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			printPreOrder(treeRecords);
		}
	}
}
void printInOrder(AVLTree<NvraRecord>* treeRecords) {
	string fileName;
	fstream outFS;

	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);

	// line with outFS literally mean writing the file out

	if (fileName.length() == 0) {											// if no input then print all the Nvra Records in trees in the requested order
		try {
			AVLTreeOrder order = AVLTreeOrder::inorder;
			AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
			if (enumerator == NULL) {
				throw new ExceptionMemoryNotAvailable();
			}

			while (enumerator->hasNext()) {
				cout << enumerator->next() << endl;
			}
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			delete enumerator;
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
		}
		catch (ExceptionMemoryNotAvailable* ea) {
			delete ea;
		}
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);						// try to open the file
		if (!outFS.is_open()) {												// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			try {
				AVLTreeOrder order = AVLTreeOrder::inorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}

				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else if (outFS.is_open()) {											// if file exis
			try {
				AVLTreeOrder order = AVLTreeOrder::inorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}

				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			printPreOrder(treeRecords);
		}
	}
}
void printPostOrder(AVLTree<NvraRecord>* treeRecords) {
	string fileName;
	fstream outFS;

	cout << "Enter output file name: ";										// ask for file name
	getline(cin, fileName);

	// line with outFS literally mean writing the file out

	if (fileName.length() == 0) {											// if no input then print all the Nvra Records in trees in the requested order
		try {
			AVLTreeOrder order = AVLTreeOrder::postorder;
			AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
			if (enumerator == NULL) {
				throw new ExceptionMemoryNotAvailable();
			}
			while (enumerator->hasNext()) {
				cout << enumerator->next() << endl;
			}
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			delete enumerator;
		}
		catch (ExceptionAVLTreeAccess* ae) {
			delete ae;
			cout << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
		}
		catch (ExceptionMemoryNotAvailable* ea) {
			delete ea;
		}
	}
	else {
		outFS.open(fileName, ios::out | ios::trunc);						// try to open the file
		if (!outFS.is_open()) {												// if file name does not exit then create one
			outFS.open(fileName, fstream::out);
			try {
				AVLTreeOrder order = AVLTreeOrder::postorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}
				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else if (outFS.is_open()) {											// if file exis
			try {
				AVLTreeOrder order = AVLTreeOrder::postorder;
				AVLTreeEnumerator<NvraRecord>* enumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
				if (enumerator == NULL) {
					throw new ExceptionMemoryNotAvailable();
				}
				while (enumerator->hasNext()) {
					outFS << enumerator->next() << endl;
				}
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
				delete enumerator;
			}
			catch (ExceptionAVLTreeAccess* ae) {
				delete ae;
				outFS << "Data lines read: " << totalLineNum << "; Valid NVRA records read: " << totalValidData << "; NVRA records in memory: " << treeRecords->getSize() << endl;
			}
			catch (ExceptionMemoryNotAvailable* ea) {
				delete ea;
			}
			outFS.close();
		}
		else {
			cout << "File is not available" << endl;						// if file somehow is not reachable then say it no available
			printPreOrder(treeRecords);
		}
	}
}
AVLTree<NvraRecord>* mergeTree(AVLTree<NvraRecord>* treeRecords, AVLTree<NvraRecord>* tempTree) {
	AVLTreeOrder order = AVLTreeOrder::inorder;
	// get both enuemartor 
	AVLTreeEnumerator<NvraRecord>* mainEnumerator = new AVLTreeEnumerator<NvraRecord>(treeRecords, order);
	AVLTreeEnumerator<NvraRecord>* tempEnumerator = new AVLTreeEnumerator<NvraRecord>(tempTree, order);

	NvraComparator* comparator;
	comparator = new NvraComparator(0);
	// new FUSION tree
	AVLTree<NvraRecord>* newTree = new AVLTree<NvraRecord>(comparator);

	if (newTree == NULL) {													// throw exception if no memeory
		throw new ExceptionMemoryNotAvailable;
	}

	NvraRecord tempRecord;
	NvraRecord mainRecord;
	while (tempEnumerator->hasNext() && mainEnumerator->hasNext()) {		// loop for while both enumerator has data
		tempRecord = tempEnumerator->peek();
		mainRecord = mainEnumerator->peek();
		if (comparator->compare(tempRecord, mainRecord) < 0) {				// if tempTree data is less then try to insert
			if (!newTree->insert(&tempRecord)) {							// if insert fail then we replace as it is the most current data
				newTree->replace(&tempRecord);
			}
			tempEnumerator->next();
		}
		else if (comparator->compare(tempRecord, mainRecord) > 0) {			// otherwise we just insert mainRecord
			newTree->insert(&mainRecord);									// if false is return then there is already a record in there
			mainEnumerator->next();
		}
		else if (comparator->compare(tempRecord, mainRecord) == 0) {		// if they are equal then
			newTree->insert(&tempRecord);									// tempTree takes priority since they are the newer record
			tempEnumerator->next();
			mainEnumerator->next();
		}
		else {
			cout << "Uh oh... problem in merging..." << endl;
		}
	}
	while (tempEnumerator->hasNext()) {										// insert in the rest of tempTree if it had more data
		tempRecord = tempEnumerator->peek();
		if (!newTree->insert(&tempRecord)) {
			newTree->replace(&tempRecord);
		}
		tempEnumerator->next();
	}	
	while (mainEnumerator->hasNext()) {										// insert in the rest of main tree if it had more records
		mainRecord = mainEnumerator->peek();
		newTree->insert(&mainRecord);
		mainEnumerator->next();
	}

	delete treeRecords;														// delete old main tree and the 2 enumerator
	delete mainEnumerator;
	delete tempEnumerator;

	return(newTree);														// return the new tree
}
AVLTree<NvraRecord>* purgeTree(AVLTree<NvraRecord>* treeRecords) {
	string firstLine;
	string rows;
	string fileName;
	ifstream inFS;
	AVLTree<NvraRecord>* tempTree;										// temporary tree for the file currently reading
	NvraComparator* tempComparator;

	tempComparator = new NvraComparator(0);
	tempTree = new AVLTree<NvraRecord>(tempComparator);					// allocate new space if there is a readable file
	if (tempTree == NULL) {												// check if there is memory
		throw new ExceptionMemoryNotAvailable();
	}

	// basically we read file in as usual

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
						if (checkData(rows) == true) {			// this will check for invalid data
							try {
								addToList(rows, tempTree);					// will attempt to add to the list THIS METHOD DEAL
							}												// WITH DUPLICATES DATA NOW
							catch (ExceptionMemoryNotAvailable* ae) {
								delete ae;
							}
						}
						totalLineNum++;										// total data read increases
						lineNum++;											// line for that file increases
					}
					if (tempTree->getSize() == 0) {
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
	} while (fileName.length() != 0 && tempTree->getSize() == 0);

	if (tempTree->getSize() != 0) {						// if the tempTree has data then there is stuff to try and remove
		AVLTreeOrder order = AVLTreeOrder::inorder;
		// get enumerator
		AVLTreeEnumerator<NvraRecord>* tempEnumerator = new AVLTreeEnumerator<NvraRecord>(tempTree, order);

		NvraRecord data;
		while (tempEnumerator->hasNext() && treeRecords->getSize() > 0) {
			data = tempEnumerator->peek();													// go through the treeRecords and delete data
			treeRecords->remove(&data);
			tempEnumerator->next();
		}
		delete tempEnumerator;
	}

	//delete tempComparator;
	delete tempTree;																		// delete tempTree
	return(treeRecords);																	// return treeRecords
}


// ALL HashTable method


// make hash table with linkedlist
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
		for (unsigned long i = 0; i < hashRecords->getBaseCapacity(); i++) {			// since all overflow have the same hash key this array with same size as
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