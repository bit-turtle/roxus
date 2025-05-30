// Implementation of some libc functions
#include <stdint.h>
#include <stddef.h>

void memcpy(void* dest, const void* src, size_t count) {
  uint8_t* destptr = (uint8_t*)dest;
  uint8_t* srcptr = (uint8_t*)src;
  while (count-- > 0)
    destptr[count] = srcptr[count];
}
