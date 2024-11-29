#include "MyList.h"
#include <stdexcept>
	
MyList::MyList(int initialSize) {
	
	size = initialSize;
	arr = new int[initialSize] {};
}

void MyList::increaseSize(int sizeToIncreaseBy) {
	if (sizeToIncreaseBy < 0) {
		throw std::invalid_argument("Size to increase by has to be positive, because we don't allow decreasing here");
	}

	size_t newSize = size + sizeToIncreaseBy;

	int* newArr = new int[newSize] {};

	for (size_t i = 0; i < size; i++) {
		newArr[i] = arr[i];
	}

	delete[] arr;
	arr = newArr;
	size = newSize;

}

MyList::~MyList() {
	delete[] arr;
	arr = nullptr;
}

int MyList::getSize() const {
	return size;
}

int& MyList::operator[](size_t index) {
	if (index >= size) {
		throw std::out_of_range("Index out of bounds.");
	}
	return arr[index];
}

