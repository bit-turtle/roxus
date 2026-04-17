#include "image.h"
#include "efi/protocols.h"
#include <stddef.h>

#include "libc.h"

efi_status_t load_image_buffer(struct efi_file_protocol *file, struct efi_graphics_output_blt_pixel** buffer, uint32_t* width, uint32_t* height) {
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

  // Allocate Image
  *buffer = malloc(pixels*sizeof(struct efi_graphics_output_blt_pixel));
  if (*buffer == NULL)
    return EFI_BUFFER_TOO_SMALL;

  // Load image
  efi_uint_t size = pixels*sizeof(struct efi_graphics_output_blt_pixel);
  status = file->read(file, &size, *buffer);
  if (status != EFI_SUCCESS) return status;

  return EFI_SUCCESS;
}

void resize_image_buffer(struct efi_graphics_output_blt_pixel* original, uint32_t width, uint32_t height, struct efi_graphics_output_blt_pixel* imagebuffer, uint32_t bufferwidth, uint32_t bufferheight) {
  // Resize Image
  uint64_t fixedpoint = 0xffff;
  uint64_t xscale = width*fixedpoint/bufferwidth;
  uint64_t yscale = height*fixedpoint/bufferheight;
  for (uint32_t x = 0; x < bufferwidth; x++) for (uint32_t y = 0; y < bufferheight; y++) {
    uint64_t bufferpixel = y*bufferwidth+x;
    uint32_t originalx = x*xscale/fixedpoint;
    uint32_t originaly = y*yscale/fixedpoint;
    uint64_t originalpixel = originaly*width+originalx;
    imagebuffer[bufferpixel] = original[originalpixel];
  }
}

void insert_image_buffer(struct efi_graphics_output_blt_pixel* source, uint32_t insertx, uint32_t inserty, uint32_t width, uint32_t height, struct efi_graphics_output_blt_pixel* dest, uint32_t destwidth, uint32_t destheight, uint32_t copyx, uint32_t copyy, uint32_t copywidth, uint32_t copyheight) {
	// Default values
	if (copywidth == 0)
		copywidth = width;
	if (copyheight == 0)
		copyheight = height;
	// Copy pixels
	for (uint32_t x = insertx; x < insertx+copywidth; x++) {
		if (x >= destwidth)
			continue;
		for (uint32_t y = inserty; y < inserty+copyheight; y++) {
			if (y >= destheight)
				continue;
			// Copy
			dest[y*destwidth+x] = source[(y-inserty+copyy)*width+(x-insertx+copyx)];
		}
	}
}

struct efi_graphics_output_blt_pixel* copy_image_buffer(struct efi_graphics_output_blt_pixel* buffer, uint32_t width, uint32_t height) {
	struct efi_graphics_output_blt_pixel* image = malloc(sizeof(struct efi_graphics_output_blt_pixel)*width*height);
	if (image == NULL)
		return NULL;
	for (uint64_t i = 0; i < width*height; i++)
		image[i] = buffer[i];
	return image;
}

void free_image(struct image* image) {
	free(image->data);
	free(image);	
}

struct image* load_image(struct efi_file_protocol* file){
	struct image* image = malloc(sizeof(struct image));
	if (image == NULL)
		return NULL;
	if (load_image_buffer(file, &image->data, &image->width, &image->height) != EFI_SUCCESS) {
		free(image);
		return NULL;
	}
	return image;
}

struct image* resize_image(struct image* source, uint32_t width, uint32_t height) {
	struct image* image = malloc(sizeof(struct image));
	if (image == NULL)
		return NULL;
	image->width = width;
	image->height = height;
	image->data = malloc(sizeof(struct efi_graphics_output_blt_pixel)*width*height);
	if (image->data == NULL) {
		free(image);
		return NULL;
	}
	resize_image_buffer(source->data, source->width, source->height, image->data, width, height);
	return image;
}

void insert_image(struct image* dest, struct image* image, uint32_t destx, uint32_t desty, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	insert_image_buffer(image->data, destx, desty, image->width, image->height, dest->data, dest->width, dest->height, x, y, width, height);
}

struct image* copy_image(struct image* source) {
	struct image* image = malloc(sizeof(struct image));
	if (image == NULL)
		return NULL;
	image->width = source->width;
	image->height = source->height;
	image->data = copy_image_buffer(source->data, source->width, source->height);
	if (image->data == NULL) {
		free(image);
		return NULL;
	}
	return image;
}
