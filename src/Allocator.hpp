#pragma once

#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

class Allocator : public Csm::ICubismAllocator {
	void* Allocate(const Csm::csmSizeType);
	void Deallocate(void*);
	void* AllocateAligned(const Csm::csmSizeType, const Csm::csmUint32);
	void DeallocateAligned(void*);
};
