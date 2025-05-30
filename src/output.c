#include "efi/system_table.h"

#include "efi/types.h"

// Parses a Decimal Integer, Returns ending position in str pointer
efi_uint_t parseInt(efi_char_t** str) {
  efi_uint_t val = 0;
  while (**str != u'\0' && **str >= u'0' && **str <= u'9') {
    val *= 10;
    val += (efi_uint_t)(**str - u'0');
    ++*str;
  }
  return val;
}
efi_uint_t getInt(efi_char_t* str) {
  return parseInt(&str);
}

efi_status_t output(struct efi_system_table* system, efi_char_t* string) {
  efi_status_t status;

  efi_char_t* buffer = string;
  bool escaped = false;
  while (*string != u'\0') {
    if (escaped) {
      if (*buffer == u'[') switch (*string) {
        // Attribute Modes
        case u'm':
          break;
        // Cursor Position
        case u'f':
        case u'H':
          {
            efi_uint_t row = 0;
            efi_uint_t column = 0;
            efi_char_t* offset = buffer+1;
            row = parseInt(&offset);
            if (*offset++ == u';') column = parseInt(&offset);
            status = system->output->setCursorPosition(system->output, row, column);
            if (status != EFI_SUCCESS) return status;
            escaped = false;
          }
          break;
        // Cursor Offset
        case u'A':
        case u'B':
        case u'C':
        case u'D':
          {
            efi_uint_t offset = getInt(buffer+1);
            efi_uint_t row = system->output->mode->cursorRow;
            efi_uint_t column = system->output->mode->cursorColumn;
          }
          break;
      }
      else {
        switch (*buffer) {
          case u'c':
            status = system->output->reset(system->output, false);
            if (status != EFI_SUCCESS) return status;
            break;
        }
        escaped = false;
      }
    }
    else if (*string == u'\e') {
      escaped = true;
      buffer = string+1;
    }
    else {
      efi_char_t character[2] = { *string, u'\0' };
      status = system->output->outputString(system->output, character);
      if (status != EFI_SUCCESS) return status;
    }
    string++;
  }

  return EFI_SUCCESS;
}
