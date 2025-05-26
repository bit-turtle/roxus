#include <stdint.h>

#include "table_header.h"
#include "handle.h"

#include "text_input.h"
#include "text_output.h"
#include "runtime_services.h"
#include "boot_services.h"
#include "config_table.h"

struct efi_system_table {
  struct efi_table_header header;
  char* vendor;
  uint32_t revision;
  // Protocols & Services
  efi_handle_t* console_in_handle;
  struct efi_simple_text_input_protocol* input;
  efi_handle_t* console_out_handle;
  struct efi_simple_text_output_protocol* output;
  struct efi_runtime_services* runtime_services;
  struct efi_boot_services* boot_services;
  // Information
  uint64_t number_of_table_entries;
  struct efi_configuration_table* config;
};
