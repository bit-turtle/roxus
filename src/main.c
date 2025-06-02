#include <stddef.h>
#include <stdint.h>

#include "efi/system_table.h"

#include "efi/types.h"
#include "efi/handle.h"

#include "bsod.h"
#include "term.h"
#include "string.h"

efi_status_t efi_main(efi_handle_t handle, struct efi_system_table* system) {
  efi_status_t status;

  // Disable Watchdog Timer
  bool watchdog = false;
  status = system->boot_services->setWatchdogTimer(0, 0x10000, 0, NULL);
  if (status != EFI_SUCCESS && status != EFI_UNSUPPORTED) watchdog = true;

  // Install GOP
  bool efigop = false;
  struct efi_graphics_output_protocol* gop;
  struct efi_guid gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  status = system->boot_services->installProtocolInterface(&handle, &gop_guid, EFI_NATIVE_INTERFACE, gop);
  if (status != EFI_SUCCESS) efigop = true;

  // Clear Screen
  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) bsod(system, status);

  // Welcome Message
  status = system->output->outputString(system->output, u"Welcome to Roxus!\n\r");
  if (status != EFI_SUCCESS) bsod(system, status);

  // Warnings
  if (watchdog || efigop) {
    if (watchdog) {
      status = system->output->outputString(system->output, u"Warning: System may Restart Unexpectedly [Watchdog Error]\n\r");
      if (status != EFI_SUCCESS) bsod(system, status);
    }
    if (efigop) {
      status = system->output->outputString(system->output, u"Info: Graphics Output Unavailable [GOP Error]\n\r");
      if (status != EFI_SUCCESS) bsod(system, status);
    }
  }
  else {
    status = system->output->outputString(system->output, u"All Systems Operational\n\r");
    if (status != EFI_SUCCESS) bsod(system, status);
  }
  status = system->output->outputString(system->output, u"\n\r");
  if (status != EFI_SUCCESS) bsod(system, status);

  // Enable Cursor
  status = system->output->enableCursor(system->output, true);
  if (status != EFI_SUCCESS) bsod(system, status);

  // Terminal
  status = term(system);
  if (status != EFI_SUCCESS) bsod(system, status);

  bsod(system, ROXUS_END);

  return EFI_SUCCESS;
}
