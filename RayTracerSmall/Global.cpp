#include "Global.h"
#include <cassert>

int checkValHeader = 0xDEADC0DE;
int checkValFooter = 0xDEADBEEF;

void* operator new(size_t size)
{
	return ::operator new(size, HeapManager::GetDefaultHeap());
}

void* operator new(size_t size, Heap* pHeap)
{
	std::cout << "New" << std::endl;
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);
	char* pMem = (char*)malloc(nRequestedBytes);
	Header* pHeader = (Header*)pMem;

	pHeader->size = size;
	pHeader->heap = pHeap;
	pHeader->checkVal = checkValHeader;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->checkVal = checkValFooter;

	std::cout << pHeader->size << std::endl;
	pHeader->heap->AddAllocation(pHeader->size + sizeof(pHeader) + sizeof(pFooter));

	void* pStartMemBlock = pMem + sizeof(Header);
	return pStartMemBlock;
}

void operator delete(void* pMem)
{
	std::cout << "Delete" << std::endl;
	Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
	assert(pHeader->checkVal == checkValHeader);

	Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);
	assert(pFooter->checkVal == checkValFooter);

	std::cout << pHeader->size << std::endl;
	pHeader->heap->RemoveAllocation(pHeader->size + sizeof(pHeader) + sizeof(pFooter));

	free(pHeader);
}