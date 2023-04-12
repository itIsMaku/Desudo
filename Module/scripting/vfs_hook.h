#pragma once
#include "../fx_game/vfs.h"

#include <string>

namespace scripting
{
extern std::string scriptToExecute;

class HookedStreamWrapper : public fx::StreamWrapper
{
public:
	virtual uint32_t Read(void* data, uint32_t size, uint32_t* bytesRead);

	virtual uint32_t Write(void* data, uint32_t size, uint32_t* bytesWritten);

	virtual uint32_t Seek(int64_t offset, int32_t origin, uint64_t* newPosition);

	virtual uint32_t GetLength(uint64_t* length);
};
} // scripting