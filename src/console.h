#include <stdint.h>
#include <stddef.h>
#include "efi/types.h"
#include "font.h"

struct terminal {
	// Output Dimensions
	efi_uint_t width;
	efi_uint_t height;
	struct efi_graphics_output_blt_pixel* output;
	// Font
	struct font* font;	
	efi_uint_t size;
	struct rendered_font* render;
	// Ring Buffer
	
}
