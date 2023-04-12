#pragma once
#include <cstdint>

namespace util
{
	inline void* get_unhooked_ptr(void* ptr)
	{
		if (*(uint8_t*)ptr == 0xF1)
			return (void*)((uint64_t)ptr + 0x1);

		return ptr;
	}
}