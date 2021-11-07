#include "Global.h"
#include <cassert>

int checkValHeader = 0xDEADC0DE;
int checkValFooter = 0xDEADBEEF;

void* operator new(size_t size, Heap* pHeap)
{
	std::cout << "new is being called" << std::endl;
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);
	char* pMem = (char*)malloc(nRequestedBytes);
	Header* pHeader = (Header*)pMem;

	pHeader->size = size;
	pHeader->checkVal = checkValHeader;
	pHeader->heap = pHeap;
	pHeap->AddAllocation(size);

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;

	pFooter->checkVal = checkValFooter;

	void* pStartMemBlock = pMem + sizeof(Header);
	return pStartMemBlock;
}

void operator delete(void* pMem)
{
	std::cout << "delete is being called" << std::endl;
	Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
	std::cout << pHeader->checkVal << std::endl;
	assert(pHeader->checkVal == checkValHeader);

	Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);
	std::cout << pFooter->checkVal << std::endl;
	assert(pFooter->checkVal == checkValFooter);

	free(pHeader);
}