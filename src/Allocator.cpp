#include "Allocator.hpp"

void* Allocator::Allocate(const Csm::csmSizeType size){
	return malloc(size);
}
void Allocator::Deallocate(void* memory){
	free(memory);
}
void* Allocator::AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment){
	size_t offset, shift, alignedAddress;
	void* allocation;
	void** preamble;

	offset = alignment - 1 + sizeof(void*);
	
	allocation = Allocate(size + (Csm::csmUint32)(offset));

	alignedAddress = (size_t)(allocation) + sizeof(void*);

	shift = alignedAddress % alignment;

	if(shift){
		alignedAddress+=(alignment - shift);
	}
	preamble = (void**)(alignedAddress);
	preamble[-1] = allocation;

	return (void*)(alignedAddress);
}

void Allocator::DeallocateAligned(void* alignedMemory){
	void** preamble;

	preamble = (void**)alignedMemory;

	Deallocate(preamble[-1]);
}
