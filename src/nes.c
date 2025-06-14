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
  uint8_t flags[10];
};

enum console_type : uint8_t {
  NES = 0,
  VS = 1,
  PLAYCHOICE_10 = 2,
  EXTENDED = 3
};

efi_status_t print_number(uint16_t number) {
  efi_char_t number_buffer[16];
  return print(itoa(number, number_buffer, 10));
}

efi_status_t run_nes_rom(struct efi_file_protocol* rom) {
  efi_status_t status;
  // Get File Info
  struct efi_file_info file_info;
  if (rom == NULL) {
    print(u"File was null!");
    return EFI_INVALID_PARAMETER;
  }
  else {
    struct efi_guid file_info_guid = EFI_FILE_INFO_ID;
    efi_uint_t file_info_size = sizeof(file_info);
    status = rom->getInfo(rom, &file_info_guid, &file_info_size, &file_info);
    if (status != EFI_SUCCESS) {
      print(u"Failed to get file info!");
      return EFI_INVALID_PARAMETER;
    }
  }

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
  uint16_t prg_rom_size = header.prg_rom_size;
  uint16_t chr_rom_size = header.chr_rom_size;
  // Detect NES 2.0
  bool nes2 = ((header.flags[1]&0xc)==0x8) ? true : false;
  if (nes2) {
    prg_rom_size |= (header.flags[2]&0xf)<<8;
    chr_rom_size |= (header.flags[2]&0xf0)<<4;
    if (0x4000*prg_rom_size+0x2000*chr_rom_size > file_info.fileSize) {
      nes2 = false;
      prg_rom_size &= 0xff;
      chr_rom_size &= 0xff;
    }
  }
  // Detect Archaic iNES
  bool archaicInes = ((header.flags[0]&0xc)==0x4) ? true : false;
  // Detect iNES
  bool ines = ((header.flags[0]&0xc)==0x0) ? true : false;
  if (ines) {
    for (int i = 0; i < 10; i++)
      if (header.flags[i] != 0)
        ines = false;
  }
  // Detect iNES 0.7
  archaicInes = (!(nes2 || archaicInes || ines)) ? true : false;
  // Display iNES Format
  if (nes2)
    print(u"Format: NES 2.0\n\r");
  else if (archaicInes)
    print(u"Format: Archaic iNES\n\r");
  else if (ines)
    print(u"Format: iNES\n\r");
  // Read Trainer
  bool trainer_present = ((header.flags[0]&(1<<2))!=0) ? true : false;
  uint8_t trainer[512];
  if (trainer_present) {
    print(u"Trainer present\n\r");
    efi_uint_t trainer_size = sizeof(trainer);
    status = rom->read(rom, &trainer_size, trainer);
    if (status != EFI_SUCCESS || trainer_size != sizeof(trainer)) {
      print(u"Failed to Read Trainer!");
      return EFI_LOAD_ERROR;
    }
  }
  // Read PRG Rom
  uint8_t* prg_rom = malloc(0x4000*prg_rom_size);
  if (prg_rom == NULL) {
    print(u"Failed to allocate memory for PRG Rom!");
    return EFI_LOAD_ERROR;
  }
  efi_uint_t prg_rom_file_size = 0x4000*prg_rom_size;
  status = rom->read(rom, &prg_rom_file_size, prg_rom);
  if (status != EFI_SUCCESS || prg_rom_file_size != 0x4000*prg_rom_size) {
    print(u"Failed to read PRG Rom!");
    free(prg_rom);
    return EFI_LOAD_ERROR;
  }
  // Read CHR Rom
  uint8_t* chr_rom = malloc(0x2000*chr_rom_size);
  if (chr_rom == NULL) {
    print(u"Failed to allocate memory for PRG Rom!");
    free(prg_rom);
    return EFI_LOAD_ERROR;
  }
  efi_uint_t chr_rom_file_size = 0x2000*chr_rom_size;
  status = rom->read(rom, &chr_rom_file_size, chr_rom);
  if (status != EFI_SUCCESS || chr_rom_file_size != 0x2000*chr_rom_size) {
    print(u"Failed to read CHR Rom!");
    free(prg_rom);
    free(chr_rom);
    return EFI_LOAD_ERROR;
  }
  // Determine Console Type
  enum console_type console_type = archaicInes ? NES : (header.flags[1]&0b11);
  // Read PlayChoice-10 Data
  uint8_t* playchoice_10 = NULL;
  if (console_type == PLAYCHOICE_10) {
    playchoice_10 = malloc(0x2000+16+16);
    if (playchoice_10 == NULL) {
      print(u"Failed to allocate memory for PlayChoice-10 Data!");
      free(prg_rom);
      free(chr_rom);
      return EFI_LOAD_ERROR;
    }
    efi_uint_t playchoice_10_size = 0x2000+16+16;
    status = rom->read(rom, &playchoice_10_size, &playchoice_10);
    if (status != EFI_SUCCESS || playchoice_10_size != 0x2000+16+16) {
      print(u"Failed to load PlayChoice-10 Data!");
      free(prg_rom);
      free(chr_rom);
      free(playchoice_10);
      return EFI_LOAD_ERROR;
    }
  }
  // Read Title (If Present)
  uint8_t title[128] = "\0";
  efi_uint_t title_size = 128;
  status = rom->read(rom, &title_size, &title);
  if (status != EFI_SUCCESS) {
    free(prg_rom);
    free(chr_rom);
    if (console_type == PLAYCHOICE_10)
      free(playchoice_10);
    return EFI_LOAD_ERROR;
  }
  if (title_size > 0) {
    print(u"Title: ");
    print_ascii(title);
  }

  // Read Mapper Value
  uint16_t mapper = 0;
  uint8_t submapper = 0;
  mapper |= (header.flags[0]&0xf0)>>4;
  if (nes2 || ines) mapper |= (header.flags[0]&0xf0)>>0;
  if (nes2) mapper |= (header.flags[1]&0xf)<<8;
  if (nes2) submapper = (header.flags[1]&0xf0)>>4;
  // Print mapper id
  print(u"Mapper: ");
  print_number(mapper);
  print(u"\n\rSubmapper: ");
  print_number(submapper);

  // Cleanup
  free(prg_rom);
  free(chr_rom);
  if (console_type == PLAYCHOICE_10)
    free(playchoice_10);
  return EFI_SUCCESS;
}
