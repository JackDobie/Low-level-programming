#pragma once

class Heap
{
public:
	Heap(const char* name);
	~Heap();
	void AddAllocation(size_t size);
	void RemoveAllocation(size_t size);
	size_t GetSize() { return m_allocatedBytes; }
	const char* GetName() { return m_name; }
private:
	size_t m_allocatedBytes = 0;
	const char* m_name = "Heap";
};