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
  efi_status_t status;
  
  // Setup Protocols
  roxus_setup(handle, system);

  // Enable Cursor
  system->output->enableCursor(system->output, true);

  // Test Mouse
  uint32_t mx = 0, my = 0, mz = 0;
  while (1) {
    struct efi_simple_pointer_state state;
    mouse_pointer->getState(mouse_pointer, &state);
    mx += state.movementX;
    my += state.movementY;
    mz += state.movementZ;
    efi_char_t buffer[16] = u"\0";
    print(u"Mouse Data");
    print(u": X="); print(itoa(mx, buffer, 10));
    print(u", Y="); print(itoa(my, buffer, 10));
    print(u", Z="); print(itoa(mz, buffer, 10));
  }

  // Terminal
  struct efi_file_protocol* root;
  filesystem->openVolume(filesystem, &root);
  status = term();
  if (status != EFI_SUCCESS) bsod(system, status);

  bsod(system, ROXUS_END);

  return EFI_SUCCESS;
}
