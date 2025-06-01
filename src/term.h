#include "efi/system_table.h"
#include "efi/types.h"

#include <stdbool.h>

efi_status_t term(struct efi_system_table* system);
efi_status_t command(struct efi_system_table* system, efi_char_t* command, bool* running);
