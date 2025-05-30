#include <stddef.h>

#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

#include "input.h"
#include "output.h"

efi_status_t efi_main(efi_handle_t* handle, struct efi_system_table* system) {
  efi_status_t status;

  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) return status;

  efi_char_t welcome[] = u"Welcome to ROXUS!\n\n\r";

  status = system->output->outputString(system->output, welcome);
  if (status != EFI_SUCCESS) return status;

  status = system->output->enableCursor(system->output, true);
  if (status != EFI_SUCCESS) return status;

  while (true) {
    status = system->output->outputString(system->output, u"> ");
    if (status != EFI_SUCCESS) return status;
    efi_char_t string[256];
    string[0] = u'\0';
    status = input(system, &string[0], 256);
    if (status != EFI_SUCCESS) return status;
    status = output(system, string);
  }

  return 0;
}
