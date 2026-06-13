#include "efi/types.h"
#include "efi/system_table.h"
#include "errors.h"

efi_status_t bsod(struct efi_system_table* system, efi_status_t error);
