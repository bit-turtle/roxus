// Implementation of some libc functions
#include <stdint.h>
#include <stddef.h>
#include "roxus.h"

void memcpy(void* dest, const void* src, size_t count) {
  uint8_t* destptr = (uint8_t*)dest;
  uint8_t* srcptr = (uint8_t*)src;
  while (count-- > 0)
    destptr[count] = srcptr[count];
}

void* memset(void *ptr, int value, size_t length) {
  while (length-- > 0)
    *(uint8_t*)ptr++ = value;
  return ptr;
}

void* malloc(size_t size) {
  void* buffer;
  efi_status_t status = boot_services->allocatePool(EFI_LOADER_DATA, (efi_uint_t)size, &buffer);
  if (status != EFI_SUCCESS)
    return NULL;
  return buffer;
}

void* calloc(size_t size) {
  void* buffer = malloc(size);
  if (buffer != NULL)
    for (void* byte = buffer; byte < buffer+size; byte++)
      *(uint8_t*)byte = 0;
  return buffer;
}

void free(void* buffer) {
  boot_services->freePool(buffer);
}
