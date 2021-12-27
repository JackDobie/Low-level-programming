#include "Heap.h"

Heap::Heap(const char* name)
{
	m_name = name;
}

Heap::~Heap()
{
	delete(m_name);
}

void Heap::AddAllocation(size_t size)
{
	m_allocatedBytes += size;
}

void Heap::RemoveAllocation(size_t size)
{
	m_allocatedBytes -= size;
}