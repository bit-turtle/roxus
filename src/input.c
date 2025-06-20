// User Input System

#include "input.h"
#include "roxus.h"

efi_status_t input(efi_char_t* string, unsigned length) {
  efi_status_t status;

  unsigned offset = 0;
  while (true) {
    struct efi_input_key key;
    status = system_table->input->readKey(system_table->input, &key);
    if (status == EFI_SUCCESS) {
      efi_char_t str[2];
      str[0] = key.unicode;
      if (key.unicode == u'\r') {
        status = system_table->output->outputString(system_table->output, u"\n\r");
        if (status != EFI_SUCCESS) return status;
        break;
      }
      else if (key.unicode == u'\0') {
        status = system_table->output->outputString(system_table->output, u"^");
        if (status != EFI_SUCCESS) return status;
        switch (key.scancode) {
          case 0x17:
            *string++ = u'\e';
            break;
          default:
            *string++ = u'^';
        }
        offset++;
      }
      else {
        if (key.unicode != u'\b' && offset < length || key.unicode == u'\b' && offset > 0) {
          status = system_table->output->outputString(system_table->output, str);
          if (status != EFI_SUCCESS) return status;
          if (key.unicode == u'\b') {
            *string-- = u'\0';
            offset--;
          }
          else {
            *string++ = key.unicode;
            offset++;
          }
        }
      }
    }
    else if (status != EFI_NOT_READY) return status;
  }
  *string = u'\0';

  return EFI_SUCCESS;
}
