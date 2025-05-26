#include <stdint.h>
#include <stdbool.h>

#include "types.h"

struct efi_input_key {
  uint16_t scancode;
  efi_char_t unicode;
};

struct efi_simple_text_input_protocol {
  efi_status_t (*reset) (struct efi_simple_text_input_protocol*, bool);
  efi_status_t (*readKey) (struct efi_simple_text_input_protocol*, struct efi_input_key*);
  void* waitForKey;
};
