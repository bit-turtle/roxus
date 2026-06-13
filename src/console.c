// console.c

#include <roxus/console.h>
#include <stdlib.h>

struct console* create_console(uint32_t width, uint32_t height, struct font* font, uint16_t size) {
	struct console* console = malloc(sizeof (struct console));
	if (console == NULL)
		return NULL;
	console->display = allocate_image(width, height);
	console->font = font;
	console->size = size;
	console->bg.red =	0x00;
	console->bg.green =	0x00;
	console->bg.blue =	0x00;
	console->bg.reserved =	0xff;
	console->fg.red =	0xff;
	console->fg.green =	0xff;
	console->fg.blue =	0xff;
	console->fg.reserved =	0xff;
	console->render = renderfont(font, size, console->fg, console->bg);
	console->x = 0;
	console->y = 0;
	return console;
}

void write_console(struct console* console, uint32_t codepoint) {
	// Handle codepoints
	switch (codepoint) {

		// Display character
		default:
			struct image character;
			character.width = console->render->width;
			character.height = console->render->height;
			character.data = getcharacter(console->render, codepoint);
			insert_image(console->display, &character, console->x, console->y, 0, 0, 0, 0);
			console->x += console->render->width;
			if (console->x > console->display->width) {
				console->x = 0;
				console->y += console->render->height;
			}
			if (console->y > console->display->height) {
				console->y -= console->render->height;
				scroll_image(console->display, console->render->height, false, 0, false);
			}
	}
}
