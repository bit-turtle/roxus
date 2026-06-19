#include <stddef.h>
#include <stdint.h>

#include <efi/system_table.h>

#include <efi/types.h>

#include <roxus/bsod.h>
#include <roxus/term.h>
#include <roxus/string.h>

#include <roxus/roxus.h>
#include <stdlib.h>

#include <roxus/console.h>
#include <roxus/image.h>

#include <utf8/utf8.h>

#include "cpptest/cpptest.h"

efi_status_t efi_main(efi_handle_t handle, struct efi_system_table* system) {
  efi_status_t status;
  
  // Setup Protocols
  roxus_setup(handle, system);

  struct efi_file_protocol* root;
  filesystem->openVolume(filesystem, &root);
  
  // Enable Cursor
 system->output->enableCursor(system->output, true);

	// C++ test
	if (cpptest() != 0x4c8)
		bsod(system, 1);

	// Terminal
  status = term();
  if (status != EFI_SUCCESS) bsod(system, status);

  bsod(system, ROXUS_END);

  return EFI_SUCCESS;
}
