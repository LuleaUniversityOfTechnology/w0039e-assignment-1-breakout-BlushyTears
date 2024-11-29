#pragma once
class MyList
{
	size_t size = 0;
	int* arr = nullptr;
	public:
		MyList(int initialSize);
		~MyList(); 
		void increaseSize(int sizeToIncreaseBy);
		int getSize() const;
		int& operator[](size_t index);
};

