#include "image.h"
#include "efi/protocols.h"
#include <stddef.h>

efi_status_t load_image(struct efi_file_protocol *file, struct efi_graphics_output_blt_pixel *buffer, efi_uint_t *bufferSize, uint32_t* width, uint32_t* height) {
  efi_status_t status;

  // Read Header
  efi_uint_t bytes;
  uint32_t header[2];
  bytes = sizeof(header);
  status = file->read(file, &bytes, header);
  if (status != EFI_SUCCESS) return status;
  *width = header[0];
  *height = header[1];
  efi_uint_t pixels = header[0]*header[1];
  /* Header Structure
   * uint32_t[2]
   * [0] Width
   * [1] Height
   * To verify file type, check the length of the file to see if the width and height sum correctly
   */

  // Get File Info
  struct efi_guid file_info_guid = EFI_FILE_INFO_ID;
  struct efi_file_info file_info;
  efi_uint_t file_info_size = sizeof(file_info);
  status = file->getInfo(file, &file_info_guid, &file_info_size, &file_info);
  if (status != EFI_SUCCESS) return status;

  // Verify file size
  if (file_info.fileSize-sizeof(header) != pixels*sizeof(struct efi_graphics_output_blt_pixel))
    return EFI_COMPROMISED_DATA;

  // Verify buffer size
  if (*bufferSize < pixels)
    return EFI_BUFFER_TOO_SMALL;

  // Load Image
  status = file->read(file, bufferSize, buffer);
  if (status != EFI_SUCCESS) return status;

  return EFI_SUCCESS;
}
