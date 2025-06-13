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
    for (uint8_t* byte = buffer; byte < buffer+size; byte++)
      *byte = 0;
  return buffer;
}

void free(void* buffer) {
  boot_services->freePool(buffer);
}
