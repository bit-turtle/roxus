// Load Image From File (RoxusImg)

#include "efi/protocols.h"

efi_status_t load_image(struct efi_file_protocol* file, struct efi_graphics_output_blt_pixel** buffer, uint32_t* width, uint32_t* height);

efi_status_t resize_image(struct efi_graphics_output_blt_pixel* original, uint32_t width, uint32_t height, struct efi_graphics_output_blt_pixel** newimage, uint32_t newwidth, uint32_t newheight);
