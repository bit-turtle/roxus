#include "efi/system_table.h"
#include "efi/protocols.h"
#include "efi/types.h"

#include <stdbool.h>

efi_status_t term();
efi_status_t command(efi_char_t* command, struct efi_file_protocol** dir, bool* running);
