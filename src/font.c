#include "font.h"
#include <stddef.h>
#include "roxus.h"
#include "libc.h"

struct font* loadfont(struct efi_file_protocol* file) {
  if (file == NULL)
    return NULL;
  
  struct efi_file_info info;
  struct efi_guid info_guid = EFI_FILE_INFO_ID;
  efi_uint_t info_size = sizeof(info);
  if (file->getInfo(file, &info_guid, &info_size, &info) != EFI_SUCCESS)
    return NULL;

  efi_uint_t bytes = 2;
  uint8_t size[2];
  file->read(file, &bytes, size);
  if (bytes != 2)
    return NULL;

  size_t entry_length = sizeof(struct character) + sizeof(uint8_t)*((size[0]*size[1] + 2) / 3);
  uint32_t entries = (info.fileSize-sizeof(struct font))/entry_length;
  struct font* font = malloc(sizeof(struct font) + entry_length*entries);
  font->width = size[0];
  font->height = size[1];
  font->entries = entries;
  font->entry_length = entry_length;

  bytes = entry_length*entries;
  file->read(file, &bytes, font->characters);
  if (bytes != entry_length*entries) {
    free(font);
    return NULL;
  }

  // Sort by Codepoint
  struct character* tmp = malloc(entry_length);
  for(int i = 1; i < font->entries; i++) {
    int j = i;
    while(j > 0 && font->characters[j*font->entry_length].codepoint < font->characters[(j-1)*font->entry_length].codepoint) {
      memcpy(tmp, &font->characters[j*font->entry_length], font->entry_length);
      memcpy(&font->characters[j*entry_length], &font->characters[(j-1)*entry_length], font->entry_length);
      memcpy(&font->characters[(j-1)*font->entry_length], tmp, font->entry_length);
      j--;
    }
  }
  free(tmp);
  
  return font;
}

uint8_t readbit(uint8_t value, uint8_t bit) {
  return (value >> (7-bit)) & 1;
}

uint8_t read3(uint8_t* base, uint16_t index) {
  base += index*3/8;
  uint8_t offset = index*3%8;
  uint8_t value = 0;
  for (int i = 0; i < 3; i++) {
    value |= readbit(*base, offset);
    offset++;
    if (offset > 7) {
      base++;
      offset = 0;
    }
  }
  return value;
}

struct rendered_font* renderfont(struct font* font, efi_uint_t size, struct efi_graphics_output_blt_pixel* color, struct efi_graphics_output_blt_pixel* background) {
  efi_char_t height = font->height/font->width;
  size_t entry_size = sizeof(struct rendered_character) + sizeof(struct efi_graphics_output_blt_pixel)*size*height;
  struct rendered_font* rendered_font = malloc(sizeof(struct rendered_font) + entry_size*font->entries);
  rendered_font->width = size;
  rendered_font->height = height;
  rendered_font->characters = font->entries;
  rendered_font->entry_size = entry_size;

  efi_uint_t half_size = (size + 1) / 2;
  
  for (uint32_t i = 0; i < font->entries; i++) {
    struct character* character = &font->characters[i*font->entry_length];
    struct efi_graphics_output_blt_pixel* render = (rendered_font->render+i*rendered_font->entry_size)->render;

    for (uint8_t y = 0; y < font->height; y++) for (uint8_t x = 0; x < font->width; x++) {
      uint8_t entry = read3(character->data, y*font->width+x);

      for (efi_uint_t renderx = size*x; renderx < size*(x+1); renderx++) for (efi_uint_t rendery = size*y; rendery < size*(y+1); rendery++) {
        struct efi_graphics_output_blt_pixel* pixel = &render[rendery*size+renderx];
        efi_uint_t relativex = renderx-size*x;
        efi_uint_t relativey = rendery-size*y;
        if (
          entry == 1 ||
          entry == 2 && relativey <= relativex ||
          entry == 3 && relativey <= size-relativex ||
          entry == 4 && relativey >= size-relativex ||
          entry == 5 && relativey >= relativex ||
          entry >= 6 && (entry == 7) != (
            relativey <= relativex + half_size &&
            relativey <= size - relativex + half_size &&
            relativey >= size - relativex - half_size &&
            relativey >= relativex - half_size
          )
        )
          *pixel = *color;
        else
          *pixel = *background;
      }
      
    }
    
  }

  return rendered_font;
}

struct rendered_character* getcharacter(struct rendered_font* rendered_font, uint32_t codepoint) {
  efi_uint_t half = rendered_font->characters/2;
  struct rendered_character* character;
  while (half > 0 && half < rendered_font->characters) {
    character = rendered_font->render+half*rendered_font->entry_size;
    if (character->codepoint == codepoint)
      return character;
    else if (character->codepoint < codepoint)
      half += (half + 1) / 2;
    else
      half -= half / 2;
  }
  return NULL;
}
