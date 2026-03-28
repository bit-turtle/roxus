/*
 * Ring Buffers
 *
 * One ring to rule them all
 * One ring to find them
 * One ring to bring them in
 * And in the darkness, bind them
 *
 * In the land of Mordor where the shadows lie.
 */

#include <stdint.h>
#include <stdbool.h>
#include "efi/types.h"
#include "libc.h"

struct ringbuffer {
	uint8_t* buffer;
	uint8_t* end;
	uint8_t* write;
	uint8_t* read;
};

// Forge and Destroy the ringbuffer
struct ringbuffer* forge_ringbuffer(efi_uint_t size);
void destroy_ringbuffer(struct ringbuffer* ringbuffer);

// Read and Write
void write_ringbuffer(uint8_t data, struct ringbuffer* ringbuffer);
uint8_t read_ringbuffer(struct ringbuffer* ringbuffer);

// Data in ringbuffer
bool data_ringbuffer(struct ringbuffer* ringbuffer);
