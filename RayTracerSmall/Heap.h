#pragma once

class Heap
{
public:
	Heap();
	~Heap();
	void AddAllocation(size_t size);
	void RemoveAllocation(size_t size);
	size_t GetSize() { return allocatedBytes; }
private:
	size_t allocatedBytes;
};