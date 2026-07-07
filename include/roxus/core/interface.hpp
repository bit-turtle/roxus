// Program Interface
#pragma once
#include <stdint.h>
#include <roxus/core/stream.hpp>
#include <roxus/core/display.hpp>

struct roxus_buffer_interface {
	uint8_t* output_buffer;	// 64kb ringbuffer
	uint8_t* input_buffer;	// 64kb ringbuffer
	uint16_t output_write_head;
	uint16_t output_read_head;
	uint16_t input_read_head;
	uint16_t input_write_head;
	uint64_t offset;

};

typedef uint8_t (*roxus_program_entry)(struct roxus_buffer_interface*);

class Interface {
private:
	uint16_t core;
	uint16_t parent;
	Stream* streams[16] = { nullptr };
	Display* displays[16] = { nullptr };
public:
	struct roxus_buffer_interface* buffers;
	
	Interface(uint16_t core, uint16_t parent, Stream* out);
	~Interface();

	void update();
};
