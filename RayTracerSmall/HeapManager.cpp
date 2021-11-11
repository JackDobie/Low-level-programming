#include "HeapManager.h"
#include "Heap.h"
#include <iostream>
#include <vector>

std::vector<Heap*> HeapManager::heaps;
Heap* HeapManager::m_defaultHeap;

HeapManager::HeapManager()
{
}

HeapManager::~HeapManager()
{
	heaps.clear();
}

Heap* HeapManager::CreateHeap(const char* name)
{
	// check if heap already exists, and create new heap if it doesn't
	Heap* heap = GetHeap(name);
	if (heap == nullptr)
	{
		heap = new Heap(name);
		heaps.push_back(heap);
		std::cout << "Heap created: " << name << std::endl;
	}
	return heap;
}

void HeapManager::CreateDefaultHeap()
{
	if (m_defaultHeap == nullptr)
	{
		m_defaultHeap = (Heap*)malloc(sizeof(Heap));
		std::cout << "Default heap created" << std::endl;
	}
}

Heap* HeapManager::GetHeap(const char* name)
{
	for (int i = 0; i < heaps.size(); i++)
	{
		if (heaps[i] != NULL)
		{
			if (heaps[i]->GetName() == name)
			{
				std::cout << "Found heap " << name << std::endl;
				return heaps[i];
			}
		}
	}
	std::cout << "Unable to find heap " << name << std::endl;
	return nullptr;
}

Heap* HeapManager::GetDefaultHeap()
{
	if (m_defaultHeap == nullptr)
	{
		CreateDefaultHeap();
	}
	return m_defaultHeap;
}
