#include <stdint.h>
#include <stddef.h>
#include "font.h"
#include "ring.h"
#include "image.h"

struct terminal {
	// Display
	struct image display;
	// Font
	struct font* font;
	uint16_t size;
	struct rendered_font* render;
	// Output buffer
	struct ringbuffer* output;
};

struct terminal* create_terminal(uint32_t width, uint32_t height, struct font* font, uint16_t size, uint16_t buffer_size);
