// Displays
#pragma once

class Display {
private:
	void* image;
	void* timer;
public:
	Display();
	// Returns 16 bit deltatime
	uint16_t frame();
};
