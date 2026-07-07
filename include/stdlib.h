#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void memcpy(void* dest, const void* src, size_t count);

void* malloc(size_t size);

void* calloc(size_t num, size_t size);

void free(void* buffer);

#ifdef __cplusplus
}
#endif
