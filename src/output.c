#include "output.h"

#include "string.h"

// Settings
efi_uint_t tab_size = 8;

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
                // Set Low Intensity Foreground Colors
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
                // Set High Intensity Foreground Colors
                case 90:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_BROWN);
                  break;
                case 91:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTRED);
                  break;
                case 92:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTGREEN);
                  break;
                case 93:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_YELLOW);
                  break;
                case 94:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTBLUE);
                  break;
                case 95:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTMAGENTA);
                  break;
                case 96:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTCYAN);
                  break;
                case 97:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_BACKGROUND | EFI_LIGHTGRAY);
                  break;
                // Set High and Low Intesity Background Colors
                case 49:
                case 40:
                case 100:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BLACK);
                  break;
                case 41:
                case 101:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_RED);
                  break;
                case 42:
                case 102:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_GREEN);
                  break;
                case 43:
                case 103:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BROWN);
                  break;
                case 44:
                case 104:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_BLUE);
                  break;
                case 45:
                case 105:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_MAGENTA);
                  break;
                case 46:
                case 106:
                  status = system->output->setAttribute(system->output, system->output->mode->attribute & EFI_FOREGROUND | EFI_BACKGROUND_CYAN);
                  break;
                case 47:
                case 107:
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
            status = system->output->clearScreen(system->output);
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
    else if (*string == u'\t') {
      efi_uint_t col = system->output->mode->cursorColumn;
      while (++col%tab_size != 0) {
        status = system->output->outputString(system->output, u" ");
        if (status != EFI_SUCCESS) return status;
      }
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
