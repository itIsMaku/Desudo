#include "pattern.h"

bool util::detail::check_pattern(uintptr_t address, uint8_t* pattern, const char* mask, size_t maskLength)
{
	for (size_t i = 0; i < maskLength; i++)
	{
		if (((uint8_t*)address)[i] != pattern[i] && mask[i] == 'x')
			return false;
	}

	return true;
}

uintptr_t util::find_pattern(uintptr_t startAddress, uintptr_t endAddress, uint8_t* pattern, const char* mask, int offset)
{
	size_t maskLen = strlen(mask);

	for (uintptr_t currentAddress = startAddress; currentAddress < endAddress; currentAddress++)
	{
		if (detail::check_pattern(currentAddress, pattern, mask, maskLen))
			return (currentAddress + offset);
	}

	return 0;
}