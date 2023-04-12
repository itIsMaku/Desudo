#include "vfs_hook.h"

namespace scripting
{
std::string scriptToExecute;

uint32_t HookedStreamWrapper::Read(void* data, uint32_t size, uint32_t* bytesRead)
{
	memcpy(data, scriptToExecute.c_str(), size);
	return 0;
}

uint32_t HookedStreamWrapper::Write(void* data, uint32_t size, uint32_t* bytesWritten)
{
	return 0;
}

uint32_t HookedStreamWrapper::Seek(int64_t offset, int32_t origin, uint64_t* newPosition)
{
	return 0;
}

uint32_t HookedStreamWrapper::GetLength(uint64_t* length)
{
	*length = scriptToExecute.length();
	return 0;
}
} // scripting