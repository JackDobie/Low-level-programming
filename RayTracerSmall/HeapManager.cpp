#include "HeapManager.h"
#include "Heap.h"
#include <iostream>
#include <vector>
#include <sstream>

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
		std::stringstream msg;
		msg << "Heap created: " << name << std::endl;
		std::cout << msg.str();
	}
	return heap;
}

void HeapManager::CreateDefaultHeap()
{
	if (m_defaultHeap == nullptr)
	{
		m_defaultHeap = (Heap*)malloc(sizeof(Heap));
		std::stringstream msg;
		msg << "Default heap created" << std::endl;
		std::cout << msg.str();
	}
}

Heap* HeapManager::GetHeap(const char* name)
{
	std::stringstream msg;
	for (int i = 0; i < heaps.size(); i++)
	{
		if (heaps[i] != NULL)
		{
			if (heaps[i]->GetName() == name)
			{
				msg << "Found heap " << name << std::endl;
				std::cout << msg.str();
				return heaps[i];
			}
		}
	}
	msg << "Unable to find heap " << name << std::endl;
	std::cout << msg.str();
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
