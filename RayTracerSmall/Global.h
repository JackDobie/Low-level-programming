#pragma once
#include <iostream>
#include "Heap.h"

struct Header
{
	int size;
	int checkVal;
	Heap* heap;
};
struct Footer
{
	int reserved;
	int checkVal;
};

void* operator new(size_t size, Heap* pHeap);
void operator delete(void* pMem);