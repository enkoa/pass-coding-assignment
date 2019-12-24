#ifndef OU_LINK_H
#define OU_LINK_H

template <typename T>
class OULink {
	template <typename meme>
	friend class OULinkedList;
	template <typename dank>
	friend class OULinkedListEnumerator;
private:
	T* data = NULL;										// pointer to data item of any type
	OULink* next = NULL;								// pointer to next link
public:
	OULink(const T* item);
	virtual ~OULink();
};

template <typename T>
OULink<T>::OULink(const T* item) {						// allocate space for the data
	data = new T(*item);
	next = NULL;
}
template <typename T>
OULink<T>::~OULink() {
	delete data;										// delete the data
	if (next != NULL) {									// then delete the next data if it is not null
		delete next;
	}
}

#endif // !OU_LINK_H


