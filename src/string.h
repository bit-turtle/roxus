// efi_char_t String Functions

#include "efi/types.h"
#include <stdbool.h>

efi_uint_t parseInt(efi_char_t** str);
efi_uint_t getInt(efi_char_t* str);

efi_char_t strcmp(efi_char_t* str1, efi_char_t* str2);
bool streq(efi_char_t* str1, efi_char_t* str2);

efi_char_t* itoa(int value, efi_char_t* result, efi_uint_t base);
