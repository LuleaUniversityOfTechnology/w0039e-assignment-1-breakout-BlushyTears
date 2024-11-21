#pragma once
class MyList
{
	size_t size = 0;
	int* arr = nullptr;
	public:
		MyList(int initialSize);
		~MyList(); 
		int getSize();
		int& operator[](size_t index);
};

