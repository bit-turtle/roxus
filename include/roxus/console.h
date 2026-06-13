#include <stdint.h>
#include <stddef.h>
#include <roxus/font.h>
#include <roxus/image.h>

struct console {
	// Display
	struct image* display;
	uint32_t x, y;
	struct efi_graphics_output_blt_pixel fg;
	struct efi_graphics_output_blt_pixel bg;
	// Font
	struct font* font;
	uint16_t size;
	struct rendered_font* render;
};

struct console* create_console(uint32_t width, uint32_t height, struct font* font, uint16_t size);

void write_console(struct console* console, uint32_t codepoint);
