#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

efi_status_t efi_main(efi_handle_t* handle, struct efi_system_table* system_table) {
  efi_status_t status;

  status = system_table->output->clearScreen(system_table->output);
  if (status != 0) return status;

  status = system_table->output->outputString(system_table->output, u"Welcome to ROXUS!");
  if (status != 0) return status;

  for (int i = 2000000000; i > 0; i--) {

  }

  return 0;
}
