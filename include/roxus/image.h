// Load Image From File (RoxusImg)
#pragma once

#include "efi/protocols.h"

// The structured image functions below are the best ones to use (uses buffer funcs underneath)

// Raw pixel images
efi_status_t load_image_buffer(struct efi_file_protocol* file, struct efi_graphics_output_blt_pixel** buffer, uint32_t* width, uint32_t* height);

void resize_image_buffer(struct efi_graphics_output_blt_pixel* original, uint32_t width, uint32_t height, struct efi_graphics_output_blt_pixel* imagebuffer, uint32_t bufferwidth, uint32_t bufferheight);

// The copy... parameters select part of the image to insert, or can be set to zero for the entire image
void insert_image_buffer(struct efi_graphics_output_blt_pixel* source, uint32_t insertx, uint32_t inserty, uint32_t width, uint32_t height, struct efi_graphics_output_blt_pixel* dest, uint32_t destwidth, uint32_t destheight, uint32_t copyx, uint32_t copyy, uint32_t copywidth, uint32_t copyheight);

struct efi_graphics_output_blt_pixel* copy_image_buffer(struct efi_graphics_output_blt_pixel* buffer, uint32_t width, uint32_t height);

// Structured images
struct image {
	// Dimensions
	uint32_t width, height;
	// Data
	struct efi_graphics_output_blt_pixel* data;
};

struct image* allocate_image(uint32_t width, uint32_t height);
void free_image(struct image* image);
struct efi_graphics_output_blt_pixel* get_pixel(struct image* image, uint32_t x, uint32_t y);
struct image* load_image(struct efi_file_protocol* file);
struct image* resize_image(struct image* image, uint32_t width, uint32_t height);
// x, y, width, and height can be set to zero for the entire image to be inserted
// Or they can be set as a selection of the source image to be inserted
void insert_image(struct image* dest, struct image* image, uint32_t destx, uint32_t desty, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void fill_image(struct image* image, struct efi_graphics_output_blt_pixel color, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void scroll_image(struct image* image, uint32_t y, bool down, uint32_t x, bool right);
void display_image(struct image* image, uint32_t x, uint32_t y);
