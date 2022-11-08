#include "aalloc.hpp"
#ifdef WINDOWS
#include <malloc.h>
#else
#include <cstdlib>
#endif

void* aalloc(size_t aling, size_t size) {
  #ifdef WINDOWS
	return _aligned_malloc(size, aling);
	#else
	return aligned_alloc(aling, size);
	#endif
}

void afree(void* data) {
  #ifdef WINDOWS
	_aligned_free(data);
	#else
	free(data);
	#endif
}
