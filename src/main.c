#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

efi_status_t efi_main(efi_handle_t* handle, struct efi_system_table* system) {
  efi_status_t status;

  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) return status;

  status = system->output->outputString(system->output, u"Welcome to ROXUS!\n\n\r> ");
  if (status != EFI_SUCCESS) return status;

  while (true) {
    struct efi_input_key key;
    status = system->input->readKey(system->input, &key);
    if (status == EFI_SUCCESS) {
      if (key.unicode == u'\n')
        status = system->output->outputString(system->output, u"\n\r> ");
      else
        status = system->output->outputString(system->output, &key.unicode);
      if (status != EFI_SUCCESS) return status;
    }
    else if (status != EFI_NOT_READY) return status;
  }

  return 0;
}
