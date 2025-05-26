#include <stdint.h>

#ifndef EFI_TABLE_HEADER
#define EFI_TABLE_HEADER

struct efi_table_header {
  uint64_t signature;
  uint32_t revision;
  uint32_t header_size;
  uint32_t crc32;
  uint32_t reserved;
};

#endif
