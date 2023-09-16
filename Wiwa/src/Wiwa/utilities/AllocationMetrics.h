 #pragma once
#include <cstdlib>
#include <Wiwa/core/Core.h>
#pragma warning(disable : 4595)
namespace Wiwa {
	// Allocation metrics
	namespace AllocationMetrics {
		static size_t allocation_count = 0;
		static size_t bytes_allocated = 0;
	};
}

// Allocation overloads
inline void* operator new(size_t size) {
	
	if (size <= 0)
		return nullptr;

	Wiwa::AllocationMetrics::allocation_count++;
	Wiwa::AllocationMetrics::bytes_allocated += size;

	return std::malloc(size);
}

inline void operator delete(void* memory, size_t size) noexcept {
	Wiwa::AllocationMetrics::allocation_count--;
	Wiwa::AllocationMetrics::bytes_allocated -= size;

	std::free(memory);
}