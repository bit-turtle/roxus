#include <stddef.h>
#include <stdint.h>

#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

#include "bsod.h"

#include "term.h"

efi_status_t efi_main(efi_handle_t* handle, struct efi_system_table* system) {
  efi_status_t status;

  // Disable Watchdog Timer
  bool watchdog = false;
  status = system->boot_services->setWatchdogTimer(0, 0x10000, 0, NULL);
  if (status != EFI_SUCCESS && status != EFI_UNSUPPORTED) watchdog = true;

  // Clear Screen
  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) bsod(system, status);

  // Welcome Message
  status = system->output->outputString(system->output, u"Welcome to Roxus!\n\r");
  if (status != EFI_SUCCESS) bsod(system, status);

  // Watchdog Warning
  if (watchdog) {
    status = system->output->outputString(system->output, u"Warning: System may Restart Unexpectedly [Watchdog Error]\n\n\r");
    if (status != EFI_SUCCESS) bsod(system, status);
  }
  else {
    status = system->output->outputString(system->output, u"All Systems Operational\n\n\r");
  }

  // Enable Cursor
  status = system->output->enableCursor(system->output, true);
  if (status != EFI_SUCCESS) bsod(system, status);

  // Terminal
  status = term(system);
  if (status != EFI_SUCCESS) bsod(system, status);

  return EFI_SUCCESS;
}
