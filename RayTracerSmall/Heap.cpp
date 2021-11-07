#include "Heap.h"

Heap::Heap()
{
	
}

Heap::~Heap()
{

}

void Heap::AddAllocation(size_t size)
{
	allocatedBytes += size;
}

void Heap::RemoveAllocation(size_t size)
{
	allocatedBytes -= size;
}