// Core Program
#include <roxus/core/program.hpp>
#include <roxus/core/interface.hpp>
#include <roxus/core/format.hpp>

uint16_t max_core_id = 0;

Program::Program() {
	id = ++max_core_id;
}

uint16_t Program::core_id() {
	return id;
}

bool Program::is_free() {
	return free;
}

bool Program::is_ready() {
	return free && ready;
}

void Program::load(uint8_t* program, size_t size) {
	if (!is_free())
		return;
	executable = program;
	// Process Header and find entry
	entry = executable;	// Temporary	
}

extern "C" {
	void roxus_core_jump(void* entry, struct roxus_buffer_interface* interface) {
		// Pass ringbuffers to the program through the C-ABI
		roxus_program_entry program = (roxus_program_entry)entry;
		program(interface);
	}
}

void Program::jump(uint16_t root) {
	if (!is_ready())
		return;
	Interface* interface = new Interface(id, 0, nullptr);
	roxus_core_jump(entry, interface->buffers);
	delete interface;
}

