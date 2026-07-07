#include <stdlib.h>
#include <stdint.h>

void* operator new(size_t size) {
	return malloc(size);
}

void operator delete(void* thing) {
	free(thing);
}

void operator delete(void* thing, size_t number) {
	free(thing);
}

void* operator new[](size_t size) {
	return malloc(size);
}

void operator delete[](void* thing) {
	free(thing);
}

void operator delete[](void* thing, size_t number) {
	free(thing);
}

extern "C" {
	int _purecall() {
		return 0;
	}
}
