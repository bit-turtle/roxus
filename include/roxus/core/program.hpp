// Core Program
#pragma once
#include <stdint.h>

class Program {
private:
	uint16_t id;
	bool free = true;
	bool ready = false;
	uint8_t* executable = nullptr;
	void* entry = nullptr;
public:
	Program();
	uint16_t core_id();
	bool is_free();
	bool is_ready();
	void load(uint8_t* program, size_t size);
	void jump(uint16_t root);
};
