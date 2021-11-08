#include "HeapManager.h"

HeapManager::HeapManager()
{
}

HeapManager::~HeapManager()
{
	heaps.clear();
}

void HeapManager::CreateHeap(const char* name)
{
}

void HeapManager::CreateDefaultHeap()
{
}

Heap* HeapManager::GetHeap(const char* name)
{
	return nullptr;
}

Heap* HeapManager::GetDefaultHeap()
{
	return nullptr;
}
