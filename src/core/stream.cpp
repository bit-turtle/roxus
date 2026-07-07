// Streams

#include <roxus/core/stream.hpp>

// Empty
Stream::~Stream() {}

// Core Stream
CoreStream::CoreStream(CoreComm* corecomm, bool coreb) {
	comm = corecomm;
	coreB = coreb;
}

CoreStream::~CoreStream() {
	comm = nullptr;
}

uint8_t* CoreStream::read() {
	uint8_t* data = buffer[0];
	for (int i = 1; i < 16; i++)
		buffer[i-1] = buffer[i];
	buffer[15] = nullptr;
	return data;
}

void CoreStream::write(uint8_t* data) {
	if (coreB)
		comm->A->buffer_write(data);
	else
		comm->B->buffer_write(data);
}

void CoreStream::buffer_write(uint8_t* data) {
	int i = 0;
	while (i < 16 && buffer[i] == nullptr)
		i++;
	// Buffer Full
	if (i == 16)
		return;
	buffer[i] = data;
}

// Core Buffer
CoreComm::CoreComm() {
	A = new CoreStream(this, false);
	B = new CoreStream(this, true);
}

CoreComm::~CoreComm() {
	delete A;
	delete B;
}

