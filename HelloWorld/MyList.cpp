#include "MyList.h"
#include <stdexcept>
	
MyList::MyList(int initialSize) {
	
	size = initialSize;
	arr = new int[initialSize] {};
}

MyList::~MyList() {
	delete[] arr;
	arr = nullptr;
}

int MyList::getSize() {
	return size;
}

int& MyList::operator[](size_t index) {
	if (index >= size) {
		throw std::out_of_range("Index out of bounds.");
	}
	return arr[index];
}

