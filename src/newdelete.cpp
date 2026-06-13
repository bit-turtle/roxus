// C++ Compatibility
#include <stddef.h>
#include <stdlib.h>

void* operator new(size_t n) noexcept {
	return malloc(n);
}

void operator delete(void* n) {
	free(n);
}
