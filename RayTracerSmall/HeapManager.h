#pragma once

#include <vector>
#include "Heap.h"

class HeapManager
{
public:
	HeapManager();
	~HeapManager();
	static void CreateHeap(const char* name);
	static void CreateDefaultHeap();
	static Heap* GetHeap(const char* name);
	static Heap* GetDefaultHeap();
private:
	std::vector<Heap*> heaps;
};