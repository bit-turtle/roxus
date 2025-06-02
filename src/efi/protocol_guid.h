// Protocol GUID
#include <stdint.h>

#include "types.h"

#ifndef EFI_PROTOCOL_GUID
#define EFI_PROTOCOL_GUID

struct efi_guid {
  uint32_t data1;
  uint16_t data2;
  uint16_t data3;
  uint8_t data4[8];
};

enum efi_interface_type {
  EFI_NATIVE_INTERFACE
};

// Protocol Structures

// GOP
struct efi_pixel_bitmask {
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  uint32_t reserved;
};
enum efi_graphics_pixel_format {
  EFI_PIXEL_FORMAT_RGBX8,
  EFI_PIXEL_FORMAT_BGRX8,
  EFI_PIXEL_FORMAT_BITMASK,
  EFI_PIXEL_FORMAT_BLT_ONLY,
  EFI_PIXEL_FORMAT_MAX
};
struct efi_graphics_output_mode_information {
  uint32_t version;
  uint32_t horizontalResolution;
  uint32_t verticalResolution;
  enum efi_graphics_pixel_format pixelFormat;
  struct efi_pixel_bitmask pixelInformation;
  uint32_t pixelsPerScanline;
};
struct efi_graphics_output_protocol_mode {
  uint32_t maxMode;
  uint32_t mode;
  struct efi_graphics_output_mode_information* info;
  efi_uint_t sizeOfInfo;
  efi_physical_address_t framebuffer;
  efi_uint_t framebufferSize;
};
struct efi_graphics_output_blt_pixel {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t reserved;
};
enum efi_graphics_output_blt_operation {
  EFI_BLT_VIDEO_FILL,
  EFI_BLT_VIDEO_TO_BLT_BUFFER,
  EFI_BLT_BUFFER_TO_VIDEO,
  EFI_BLT_VIDEO_TO_VIDEO,
  EFI_GRAPHICS_OUTPUT_BLT_OPERATION_MAX
};

struct efi_graphics_output_protocol {
  efi_status_t (*queryMode)(struct efi_graphics_output_protocol*, uint32_t, efi_uint_t*, struct efi_graphics_output_mode_information**);
  efi_status_t (*setMode)(struct efi_graphics_output_protocol*, uint32_t);
  efi_status_t (*blt)(struct efi_graphics_output_protocol*, struct efi_graphics_output_blt_pixel*, enum efi_graphics_output_blt_operation, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t);
  struct efi_graphics_output_protocol_mode* mode;
};

// GUID List
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
 {0x9042a9de,0x23dc,0x4a38,\
  {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

#endif
