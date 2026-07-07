// Streams
#pragma once
#include <stdint.h>

// First two bytes of data is the length
// Length of 0 means a null terminated string
// Lengths of 0xfff0 and above are error codes and should have no data
// Code 0xffff means there is no new data to read, but no error occurred
// Base Stream Class
class Stream {
public:
	virtual ~Stream() = 0;

	// Read
	virtual uint8_t* read() = 0;

	// Write
	virtual void write(uint8_t* data) = 0;
};

// Reads and writes to files using the stream system
class FileStream : public Stream {
	FileStream(char* a);
};

// Inter Core Communication Streams
class CoreComm;	// Forward declaration
class CoreStream : public Stream {
private:
	CoreComm* comm = nullptr;
	bool coreB;
	uint8_t* buffer[16] = { nullptr };
public:
	CoreStream(CoreComm* comm, bool coreB);
	~CoreStream();
	uint8_t* read();
	void write(uint8_t* data);
	// Used to communicate between the two CoreStream instances in a CoreComm
	void buffer_write(uint8_t* data);
};

// Data is only copied once it is requested, up to 16 packets can be kept track of at a time, less than 0xffff bytes total
class CoreComm {
public:
	CoreStream* A = nullptr;
	CoreStream* B = nullptr;
	CoreComm();
	~CoreComm();
};


