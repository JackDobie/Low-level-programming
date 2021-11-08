#include "Heap.h"

Heap::Heap(char* name)
{
	m_name = name;
}

Heap::~Heap()
{

}

void Heap::AddAllocation(size_t size)
{
	m_allocatedBytes += size;
}

void Heap::RemoveAllocation(size_t size)
{
	m_allocatedBytes -= size;
}