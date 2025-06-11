// Load Image From File (RoxusImg)

#include "efi/protocols.h"

efi_status_t load_image(struct efi_file_protocol* file, struct efi_graphics_output_blt_pixel* buffer, efi_uint_t* bufferSize, uint32_t* width, uint32_t* height);
