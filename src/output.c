#include "output.h"

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
          {
            efi_char_t* offset = buffer+1;
            do {
              efi_uint_t code = parseInt(&offset);
              switch (code) {
                // Set Foreground Colors
                case 30:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_BLACK);
                  break;
                case 31:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_RED);
                  break;
                case 32:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_GREEN);
                  break;
                case 33:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_YELLOW);
                  break;
                case 34:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_BLUE);
                  break;
                case 35:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_MAGENTA);
                  break;
                case 36:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_CYAN);
                  break;
                case 37:
                case 39:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_WHITE);
                  break;
                // Set Background Colors
                case 49:
                case 40:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BLACK);
                  break;
                case 41:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_RED);
                  break;
                case 42:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_GREEN);
                  break;
                case 43:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BROWN);
                  break;
                case 44:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BLUE);
                  break;
                case 45:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_MAGENTA);
                  break;
                case 46:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_CYAN);
                  break;
                case 47:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_LIGHTGRAY);
                  break;
              }
              if (status != EFI_SUCCESS) return status;
            }
            while (*offset++ == u';');
            escaped = false;
          }
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
            efi_uint_t offset = 1;  // Defaults to 1
            if (buffer+1 != string) offset = getInt(buffer+1);
            efi_uint_t row = system->output->mode->cursorRow;
            efi_uint_t column = system->output->mode->cursorColumn;
            switch (*string) {
              case u'A':
                row -= offset;
                break;
              case u'B':
                row += offset;
                break;
              case u'C':
                column += offset;
                break;
              case u'D':
                column -= offset;
                break;
            }
            status = system->output->setCursorPosition(system->output, row, column);
            if (status != EFI_SUCCESS) return status;
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
