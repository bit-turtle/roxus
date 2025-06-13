#include "nes.h"
#include "efi/types.h"
#include "roxus.h"
#include "libc.h"
#include "string.h"
#include <stddef.h>

#define INES_MAGIC "NES\x1a"

struct ines_header {
  uint8_t magic[4];
  uint8_t prg_rom_size;
  uint8_t chr_rom_size;
  uint8_t flags[5];
  uint8_t padding[5];
};

enum ines_mapper: uint16_t {
  NOT_SUPPORTED_YET
};

efi_status_t run_nes_rom(struct efi_file_protocol* rom) {
  efi_status_t status;
  // Read Header
  struct ines_header header;
  efi_uint_t header_size = sizeof(header);
  status = rom->read(rom, &header_size, &header);
  if (status != EFI_SUCCESS || header_size != sizeof(header)) {
    print(u"Failed to read iNES Header!");
    return EFI_LOAD_ERROR;
  }
  // Verify magic
  for (int i = 0; i < 4; i++) if (header.magic[i] != INES_MAGIC[i]) {
    print(u"iNES Header magic value incorrect!");
    return EFI_LOAD_ERROR;
  }
  // Read Trainer
  bool trainer_present = ((header.flags[0]&(1<<2))!=0) ? true : false;
  uint8_t* trainer = NULL;
  if (trainer_present) {
    print(u"Trainer present, reading trainer...\n\r");
    trainer = malloc(sizeof(uint8_t[512]));
    if (trainer == NULL) {
      print(u"Failed to allocate memory for trainer!");
      goto exit;
    }
    efi_uint_t trainer_size = sizeof(uint8_t[512]);
    status = rom->read(rom, &trainer_size, trainer);
    if (status != EFI_SUCCESS || trainer_size != sizeof(trainer)) {
      print(u"Failed to Read Trainer!");
      goto exit;
    }
  }
  // Read PRG Rom
  uint8_t* prg_rom = malloc(0x4000*header.prg_rom_size);
  if (prg_rom == NULL) {
    print(u"Failed to allocate memory for PRG Rom!");
    goto exit;
  }
  efi_uint_t prg_rom_size = 0x4000*header.prg_rom_size;
  status = rom->read(rom, &prg_rom_size, prg_rom);
  if (status != EFI_SUCCESS || prg_rom_size != 0x4000*header.prg_rom_size) {
    print(u"Failed to read PRG Rom!");
    goto exit;
  }
  // Read CHR Rom
  bool chr_rom_present = (header.chr_rom_size != 0) ? true : false; // false: chr_ram is used
  uint8_t* chr_rom = NULL;
  if (chr_rom_present) {
    chr_rom = malloc(0x2000*header.chr_rom_size);
    if (chr_rom == NULL) {
      print(u"Failed to allocate memory for CHR Rom!");
      goto exit;
    }
  }
  // Read PlayChoice-10 Data
  bool playchoice_10_present = ((header.flags[1]&(1<<1))!=0) ? true : false;
  uint8_t* playchoice_10 = NULL;
  if (playchoice_10_present) {
    playchoice_10 = malloc(0x2000);
    if (playchoice_10 == NULL) {
      print(u"Failed to allocate memory for PlayChoice-10 Data!");
      goto exit;
    }
    efi_uint_t playchoice_10_size = 0x2000;
    status = rom->read(rom, &playchoice_10_size, &playchoice_10);
    if (status != EFI_SUCCESS || playchoice_10_size != 0x2000) {
      print(u"Failed to load PlayChoice-10 Data!");
      goto exit;
    }
  }
  // Detect NES 2.0
  bool nes2 = ((header.flags[1]&(1<<3))==1&&(header.flags[1]&(1<<2))==0) ? true : false;
  // Read Mapper Number
  enum ines_mapper mapper = 0;
  mapper |= (header.flags[0]&0xf0)>>4;
  mapper |= (header.flags[0]&0xf0)>>0;

  // Print mapper id
  efi_char_t buffer[16];
  print(itoa(mapper, &buffer, 10));

  // Dealloc and Exit
exit:
  free(trainer);
  free(prg_rom);
  free(chr_rom);
  free(playchoice_10);
  return status;
}
