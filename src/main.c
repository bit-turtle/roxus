#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

#include "input.h"

efi_status_t efi_main(efi_handle_t* handle, struct efi_system_table* system) {
  efi_status_t status;

  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) return status;

  status = system->output->outputString(system->output, u"Welcome to ROXUS!\n\n\r> ");
  if (status != EFI_SUCCESS) return status;

  while (true) {
    efi_char_t string[256];
    status = input(system, &string[0], 256);
    if (status != EFI_SUCCESS) return status;
    status = system->output->outputString(system->output, u"> ");
    if (status != EFI_SUCCESS) return status;
  }

  return 0;
}
