// The One Ring

#include "ring.h"

// Forge and Destroy
struct ringbuffer* forge_ringbuffer(efi_uint_t size) {
	struct ringbuffer* ring = (struct ringbuffer*)malloc(sizeof (struct ringbuffer));
	void* buffer = malloc(size);
	ring->buffer = buffer;
	ring->end = buffer+size;
	ring->write = buffer;
	ring->read = buffer;
	return ring;
}
void destroy_ringbuffer(struct ringbuffer* ringbuffer) {
	free(ringbuffer->buffer);
	free(ringbuffer);
}

// Read and Write
void write_ringbuffer(uint8_t data, struct ringbuffer* ringbuffer) {
	// Write and increment pointer
	*ringbuffer->write++ = data;
	// Loop if the end was reached
	if (ringbuffer->write == ringbuffer->end)
		ringbuffer->write = ringbuffer->buffer;
}
uint8_t read_ringbuffer(struct ringbuffer* ringbuffer) {
	// Read and increment pointer
	uint8_t data = *ringbuffer->read++;
	// Loop if the end was reached
	if (ringbuffer->read == ringbuffer->end)
		ringbuffer->read = ringbuffer->buffer;
	// Return read data
	return data;
}
bool data_ringbuffer(struct ringbuffer* ringbuffer) {
	return ringbuffer->write != ringbuffer->read;
}
