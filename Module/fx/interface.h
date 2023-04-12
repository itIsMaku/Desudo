#pragma once
#include <atomic>

struct guid_t
{
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
};

namespace fx
{
class IBase
{
public:
	// Return 0 so we can create instances of classes that extend from this one, like the hooked Stream used for execution.
	// It's a little hacky, but it works. Created clases should be deleted after use.

	virtual uint32_t QueryInterface(const guid_t& riid, void** outObject)
	{
		return 0;
	};

	virtual uint32_t AddRef()
	{
		return 0;
	};

	virtual uint32_t Release()
	{
		return 0;
	};
};

intptr_t findFirstImpl(const guid_t& iid, guid_t* clsid);

int32_t findNextImpl(intptr_t findHandle, guid_t* clsid);

void findImplClose(intptr_t findHandle);

uint32_t createObjectInstance(const guid_t& guid, const guid_t& iid, void** objectRef);
} // fx