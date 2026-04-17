// console.c

#include "console.h"

struct terminal* create_terminal(uint32_t width, uint32_t height, struct font* font, uint16_t size, uint16_t buffer_size) {
	struct terminal* terminal = malloc(sizeof (struct terminal));
	if (terminal == NULL)
		return NULL;
	terminal->display.width = width;
	terminal->display.height = height;
	terminal->display.data = malloc(width*height*(sizeof (struct efi_graphics_output_blt_pixel)));
	terminal->font = font;
	terminal->size = size;
	terminal->output = forge_ringbuffer(0xff);
}
