#pragma once

#include <vector>

class Heap;

class HeapManager
{
public:
	HeapManager();
	~HeapManager();
	static Heap* CreateHeap(const char* name);
	static void CreateDefaultHeap();
	static Heap* GetHeap(const char* name);
	static Heap* GetDefaultHeap();
private:
	static std::vector<Heap*> heaps;
	static Heap* m_defaultHeap;
};