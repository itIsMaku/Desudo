#pragma once
#include <cstdint>
#include <cstring>

namespace util
{
	namespace detail
	{
		bool check_pattern(uintptr_t address, uint8_t* pattern, const char* mask, size_t maskLength);
	}

	uintptr_t find_pattern(uintptr_t startAddress, uintptr_t endAddress, uint8_t* pattern, const char* mask, int offset = 0);
}