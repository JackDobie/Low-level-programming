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
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);
	char* pMem = (char*)malloc(nRequestedBytes);
	Header* pHeader = (Header*)pMem;

	pHeader->size = size;
	pHeader->heap = pHeap;
	pHeader->heap->AddAllocation(size);
	pHeader->checkVal = checkValHeader;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;

	pFooter->checkVal = checkValFooter;

	void* pStartMemBlock = pMem + sizeof(Header);
	return pStartMemBlock;
}

void operator delete(void* pMem)
{
	Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
	assert(pHeader->checkVal == checkValHeader);
	pHeader->heap->RemoveAllocation(pHeader->size);

	Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);
	assert(pFooter->checkVal == checkValFooter);

	free(pHeader);
}