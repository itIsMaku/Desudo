#pragma once
#include "../fx/ref_container.h"
#include "../fx/interface.h"

#include <string>

namespace fx
{
class Stream : public fwRefCountable
{
public:
	size_t Read(void* buffer, size_t length);

	uint64_t GetLength();

public:
	static fwRefContainer<Stream> open_read(const std::string& path);
};

class StreamWrapper : public IBase
{
public:
	virtual uint32_t Read(void* data, uint32_t size, uint32_t* bytesRead) = 0;

	virtual uint32_t Write(void* data, uint32_t size, uint32_t* bytesWritten) = 0;

	virtual uint32_t Seek(int64_t offset, int32_t origin, uint64_t* newPosition) = 0;

	virtual uint32_t GetLength(uint64_t* length) = 0;
};
} // fx