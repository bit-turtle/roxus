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

  // Locate GOP
  bool efigop = false;
  struct efi_graphics_output_protocol* gop = NULL;
  {
    struct efi_guid gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = system->boot_services->locateProtocol(&gop_guid, NULL, &gop);
    if (status != EFI_SUCCESS) efigop = true;
  }

  // Find Best Text Mode
  {
    efi_uint_t best_width = 0, best_height = 0;
    efi_uint_t best_mode = 0;
    for (efi_uint_t mode = 0; mode < system->output->mode->maxMode; mode++) {
      efi_uint_t width, height;
      status = system->output->queryMode(system->output, mode, &width, &height);
      if (status == EFI_SUCCESS)
      if (width > best_width || height > best_height ) {
        best_width = width;
        best_height = height;
        best_mode = mode;
      }
    }
    status = system->output->setMode(system->output, best_mode);
    if (status != EFI_SUCCESS) bsod(system, status);
  }

  // Start GOP (If not started)
  {
    struct efi_graphics_output_mode_information* info;
    efi_uint_t infosize, nativeMode = 0;
    status = gop->queryMode(gop, gop->mode==NULL?0:gop->mode->mode, &infosize, &info);
    if (status == EFI_NOT_STARTED) {
      // default to mode zero
      status = gop->setMode(gop, 0);
      if (status != EFI_SUCCESS) efigop = true;
    }
  }

  // Clear Screen
  status = system->output->clearScreen(system->output);
  if (status != EFI_SUCCESS) bsod(system, status);

  // Welcome Message
  status = system->output->outputString(system->output, u"Welcome to Roxus!\n\r");
  if (status != EFI_SUCCESS) bsod(system, status);

  // Load Filesystem
  bool fserror = true;
  struct efi_simple_file_system_protocol* fs = NULL;
  struct efi_file_protocol* root = NULL;
  {
    struct efi_guid simple_fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    status = system->boot_services->locateProtocol(&simple_fs_guid, NULL, &fs);
    if (status == EFI_SUCCESS) {
      status = fs->openVolume(fs, &root);
      if (status == EFI_SUCCESS)
        fserror = false;
    }
  }

  // Warnings
  if (watchdog || efigop || fserror) {
    if (watchdog) {
      status = system->output->outputString(system->output, u"Warning: System may Restart Unexpectedly [Watchdog Error]\n\r");
      if (status != EFI_SUCCESS) bsod(system, status);
    }
    if (efigop) {
      status = system->output->outputString(system->output, u"Info: Graphics Output Unavailable [GOP Error]\n\r");
      if (status != EFI_SUCCESS) bsod(system, status);
    }
    if (fserror) {
      status = system->output->outputString(system->output, u"Error: Failed to open Filesystem Volume\n\r");
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
  status = term(system, gop, root);
  if (status != EFI_SUCCESS) bsod(system, status);

  bsod(system, ROXUS_END);

  return EFI_SUCCESS;
}
