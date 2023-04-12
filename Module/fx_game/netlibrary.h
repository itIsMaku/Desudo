#pragma once
#include "../fx/event.h"

namespace fx
{
class NetLibrary
{
private:
	char pad_0x0000[0x3F0];

public:
	fwEvent<std::string&, std::string&> OnTriggerServerEvent;
};
} // fx