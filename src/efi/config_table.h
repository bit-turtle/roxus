#include <stdint.h>

struct efi_configuration_table {
  __uint128_t vendorGuid;
  void* vendorTable;
};
