#include <stddef.h>
#include <stdint.h>

#include "efi/system_table.h"

#include "efi/types.h"

#include "bsod.h"
#include "term.h"
#include "string.h"

#include "roxus.h"
#include "libc.h"

#include "ring.h"

efi_status_t efi_main(efi_handle_t handle, struct efi_system_table* system) {
  efi_status_t status;
  
  // Setup Protocols
  roxus_setup(handle, system);

  // Enable Cursor
  system->output->enableCursor(system->output, true);

  // Terminal
  struct efi_file_protocol* root;
  filesystem->openVolume(filesystem, &root);
  status = term();
  if (status != EFI_SUCCESS) bsod(system, status);

  bsod(system, ROXUS_END);

  return EFI_SUCCESS;
}
