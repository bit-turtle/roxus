#include "font.h"
#include <stddef.h>
#include "roxus.h"
#include "libc.h"
#include "string.h"

struct font* loadfont(struct efi_file_protocol* file) {
  if (file == NULL)
    return NULL;
  
  struct efi_file_info info;
  struct efi_guid info_guid = EFI_FILE_INFO_ID;
  efi_uint_t info_size = sizeof(info);
  if (file->getInfo(file, &info_guid, &info_size, &info) != EFI_SUCCESS)
    return NULL;

  if (info.fileSize < 2)
    return NULL;

  uint8_t dimensions[2];

  efi_uint_t bytes = sizeof(dimensions);
  file->read(file, &bytes, dimensions);
  if (bytes != 2)
    return NULL;

  uint8_t width = dimensions[0], height = dimensions[1];

  size_t entry_size = sizeof(struct character) + sizeof(uint8_t)*((width*height*3+7)/8);
  size_t entries = (info.fileSize - sizeof(dimensions)) / entry_size;

  struct font* font = malloc(sizeof(struct font) + sizeof(struct character*)*entries);
  font->width = width;
  font->height = height;
  font->entry_size = entry_size;

  for (font->entries = 0; font->entries < entries; font->entries++) {
    struct character* character = malloc(entry_size);
    bytes = entry_size;
    file->read(file, &bytes, character);
    if (bytes != entry_size) {
      while (font->entries > 0)
        free(font->characters[font->entries]);
      free(font);
      return NULL;
    }

    // Reverse bytes
    uint8_t* byte = (uint8_t*)&character->codepoint;
    uint8_t tmp = byte[0];
    byte[0] = byte[3];
    byte[3] = tmp;
    tmp = byte[1];
    byte[1] = byte[2];
    byte[2] = tmp;
    
    font->characters[font->entries] = character;
  }

  // Sort by Codepoint
  for (int step = 1; step < font->entries; step++) {
    struct character* key = font->characters[step];
    int check = step - 1;

    while (check >= 0 && key->codepoint < font->characters[check]->codepoint) {
      font->characters[check + 1] = font->characters[check];
      check--;
    }
    font->characters[check + 1] = key;
  }

  // Success
  return font;
}

uint8_t readbit(uint8_t value, uint8_t bit) {
  return (value >> (7-bit)) & 1;
}

uint8_t read3(uint8_t* base, size_t index) {
  base += (index*3) / 8;
  uint8_t offset = (index*3) % 8;
  uint8_t value = 0;
  for (int i = 0; i < 3; i++) {
    value |= readbit(*base, offset) << i;
    offset++;
    if (offset > 7) {
      base++;
      offset = 0;
    }
  }
  return value;
}

struct rendered_font* renderfont(struct font* font, efi_uint_t size, struct efi_graphics_output_blt_pixel color, struct efi_graphics_output_blt_pixel background) {

  struct rendered_font* render = malloc(sizeof(struct rendered_font) + sizeof(struct efi_graphics_output_blt_pixel*)*font->entries);
  efi_uint_t tile_size = size/font->width;
  render->font = font;
  render->width = font->width*tile_size;
  render->height = font->height*tile_size;

  // Render Font
  for (size_t entry = 0; entry < font->entries; entry++) {
    render->render[entry] = malloc(render->width*render->height*sizeof(struct efi_graphics_output_blt_pixel));
    for (uint8_t y = 0; y < font->height; y++) for (uint8_t x = 0; x < font->width; x++) {

      uint8_t tile = read3(font->characters[entry]->data, x+y*font->width);
      efi_uint_t offsetx = x*tile_size;
      efi_uint_t offsety = y*tile_size;
      
      // Render Pixel
      for (efi_uint_t y = 0; y < tile_size; y++) for (efi_uint_t x = 0; x < tile_size; x++) {
        if (
          tile == 1 ||
          tile == 2 && y >= tile_size-x-1 ||
          tile == 3 && y >= x ||
          tile == 4 && y <= x ||
          tile == 5 && y <= tile_size-x-1 ||
          tile >= 6 && (tile == 7) != (
            // Top Right
            (x>=tile_size/2 && y<tile_size/2) && (y >= x+1 - tile_size/2) ||
            // Bottom Right
            (x>=tile_size/2 && y>=tile_size/2) && (y < tile_size - x + tile_size/2 - 1) ||
            // Top Left
            (x<tile_size/2 && y<tile_size/2) && (y >= tile_size - x - tile_size/2) ||
            // Bottom Left
            (x<tile_size/2 && y>=tile_size/2) && (y < x + tile_size/2)
          )
        ) {
          render->render[entry][ (x+offsetx) + (y+offsety)*size ] = color;
        }
        else {
          render->render[entry][ (x+offsetx) + (y+offsety)*size ] = background;
        }
      }
      
    }
  }

  return render;
}

struct efi_graphics_output_blt_pixel* getcharacter(struct rendered_font* render, uint32_t codepoint) {
  // Binary Search
  int low = 0;
  int high = render->font->entries - 1;
  while (low <= high) {
    int mid = low + (high - low) / 2;

    if (render->font->characters[mid]->codepoint == codepoint)
        return render->render[mid];

    if (render->font->characters[mid]->codepoint < codepoint)
        low = mid + 1;

    else
        high = mid - 1;
  }
  return NULL;
}
