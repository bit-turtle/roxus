#include "string.h"

efi_char_t strcmp(efi_char_t* str1, efi_char_t* str2) {

  while (*str1 && *str1++ == *str2++);

  return *str1 - *str2;

}

bool streq(efi_char_t* str1, efi_char_t* str2) {
  return strcmp(str1, str2) == 0;
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 * Modified to use efi_char_t
 */
efi_char_t* itoa(int value, efi_char_t* result, efi_uint_t base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    efi_char_t* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = u"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';

    // Reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
