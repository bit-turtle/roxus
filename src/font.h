#include <stdint.h>
#include <stddef.h>
#include "efi/protocols.h"

struct character {
  uint32_t codepoint;
  uint8_t data[];
};

struct font {
  uint8_t width;
  uint8_t height;
  size_t entries;
  size_t entry_size;
  struct character* characters[];
};

struct rendered_font {
  struct font* font;
  uint32_t width;
  uint32_t height;
  struct efi_graphics_output_blt_pixel* render[];
};

struct font* loadfont(struct efi_file_protocol* file);

struct rendered_font* renderfont(struct font* font, uint16_t tile_size, struct efi_graphics_output_blt_pixel color, struct efi_graphics_output_blt_pixel background);

struct efi_graphics_output_blt_pixel* getcharacter(struct rendered_font* rendered_font, uint32_t codepoint);

struct efi_graphics_output_blt_pixel* displaytext(struct rendered_font* rendered_font, uint16_t width, uint16_t height, efi_char_t* text, efi_uint_t scroll);
