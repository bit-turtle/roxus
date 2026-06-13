#include <stdint.h>
#include <stdbool.h>

#include "types.h"

#ifndef EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

struct simple_text_output_mode {
  int32_t maxMode;
  // Current
  int32_t mode;
  int32_t attribute;
  int32_t cursorColumn;
  int32_t cursorRow;
  bool cursorVisible;
};

struct efi_simple_text_output_protocol {
  efi_status_t (*reset) (struct efi_simple_text_output_protocol*, bool);
  efi_status_t (*outputString) (struct efi_simple_text_output_protocol*, efi_char_t*);
  efi_status_t (*testString) (struct efi_simple_text_output_protocol*, efi_char_t*);
  efi_status_t (*queryMode) (struct efi_simple_text_output_protocol*, efi_uint_t, efi_uint_t*, efi_uint_t*);
  efi_status_t (*setMode) (struct efi_simple_text_output_protocol*, efi_uint_t);
  efi_status_t (*setAttribute) (struct efi_simple_text_output_protocol*, efi_uint_t);
  efi_status_t (*clearScreen) (struct efi_simple_text_output_protocol*);
  efi_status_t (*setCursorPosition) (struct efi_simple_text_output_protocol*, efi_uint_t, efi_uint_t);
  efi_status_t (*enableCursor) (struct efi_simple_text_output_protocol*, bool);
  struct simple_text_output_mode* mode;
};

#endif
