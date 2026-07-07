// Program Interface

#include <stdlib.h>
#include <roxus/core/interface.hpp>

Interface::Interface(uint16_t core_id, uint16_t parent_id, Stream* out) {
	// Variable Initialization
	core = core_id;
	parent = parent_id;
	streams[0] = out;
	// Allocate Buffer Interface
	buffers = (struct roxus_buffer_interface*)malloc(sizeof(struct roxus_buffer_interface));
	if (buffers != nullptr) {
		buffers->output_buffer = (uint8_t*)malloc(sizeof(uint8_t)*0xffff);
		if (buffers->output_buffer == nullptr) {
			free(buffers);
			buffers = nullptr;
			return;
		}
		buffers->input_buffer = (uint8_t*)malloc(sizeof(uint8_t)*0xffff);
		if (buffers->input_buffer == nullptr) {
			free(buffers->output_buffer);
			free(buffers);
			return;
		}
	}
}

Interface::~Interface() {
	// Free Buffer Interface
	if (buffers != nullptr) {
		if (buffers->output_buffer != nullptr)
			free(buffers->output_buffer);
		if (buffers->input_buffer != nullptr)
			free(buffers->input_buffer);
		free(buffers);
	}
	// Delete Streams and Displays
	for (uint8_t i = 0; i < 16; i++) {
		if (streams[i] != nullptr)
			delete streams[i];
		if (displays[i] != nullptr)
			delete displays[i];
	}
}

uint64_t read_u64(struct roxus_buffer_interface* buffers) {
	uint64_t val = 0;
	for (int i = 0; i < 8; i++) {
		val <<= 8;
		val |= buffers->output_buffer[++buffers->output_read_head];
	}
	return val;
}
uint64_t read_u16(struct roxus_buffer_interface* buffers) {
	uint64_t val = 0;
	for (int i = 0; i < 2; i++) {
		val <<= 8;
		val |= buffers->output_buffer[++buffers->output_read_head];
	}
	return val;
}

void program_output(uint8_t stream_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void program_input(uint8_t stream_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void allocate_memory(uint8_t variant, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
	// Free Memory
	if (variant == 0xf && delta_out > 8) {
		void* pointer = &buffers->output_buffer[++buffers->output_read_head];
		buffers->output_read_head += 8;
		if (pointer != nullptr) free(pointer);
	}
}

void boot_core(uint8_t stream_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void configuration_entry(uint8_t variant, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void create_display(uint8_t display_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void edit_display_position(uint8_t display_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void push_display_frame(uint8_t display_id, uint16_t delta_out, struct roxus_buffer_interface* buffers) {
}

void Interface::update() {
	if (buffers == nullptr)
		return;
	uint16_t delta_out = buffers->output_write_head - buffers->output_read_head;
	if (delta_out == 0)
		return;
	uint8_t command = buffers->output_buffer[buffers->output_read_head];
	uint8_t series = command & 0xf0;
	uint8_t variant = command & 0x0f;
	switch (series) {
		case 0xa0:
			allocate_memory(variant, delta_out, buffers);
			break;
		case 0xb0:
			boot_core(variant, delta_out, buffers);
			break;
		case 0xc0:
			configuration_entry(variant, delta_out, buffers);
			break;
		case 0xd0:
			create_display(variant, delta_out, buffers);
			break;
		case 0xe0:
			edit_display_position(variant, delta_out, buffers);
			break;
		case 0xf0:
			push_display_frame(variant, delta_out, buffers);
			break;
		// Skip Unknown Command Series
		default:
			buffers->output_read_head++;
	}
}
