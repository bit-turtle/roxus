#include <stdint.h>

#ifndef EFI_CONFIGURATION_TABLE
#define EFI_CONFIGURATION_TABLE

struct efi_configuration_table {
  __uint128_t vendorGuid;
  void* vendorTable;
};

#endif
