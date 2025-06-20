#include <stddef.h>
#include <stdint.h>

#include "efi/system_table.h"

#include "efi/types.h"

#include "bsod.h"
#include "term.h"
#include "string.h"

#include "roxus.h"
#include "libc.h"

efi_status_t efi_main(efi_handle_t handle, struct efi_system_table* system) {
  // Setup Protocols
  roxus_setup(handle, system);

  efi_status_t status;

  efi_char_t* test = malloc(16*sizeof(efi_char_t));
  if (test != NULL) test = u"Wow it works";
  else print(u"Womp womp");

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
