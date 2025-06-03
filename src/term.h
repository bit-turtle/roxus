#include "efi/system_table.h"
#include "efi/protocol_guid.h"
#include "efi/types.h"

#include <stdbool.h>

efi_status_t term(struct efi_system_table* system, struct efi_graphics_output_protocol* gop);
efi_status_t command(struct efi_system_table* system, struct efi_graphics_output_protocol* gop, efi_char_t* command, bool* running);
