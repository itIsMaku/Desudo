#include "component.h"

#include "../include/xorstr/xorstr.hpp"

#include <Windows.h>

namespace fx
{
ComponentRegistryImpl* ComponentRegistry::get_impl()
{
	static ComponentRegistryImpl*(*CoreGetComponentRegistry)() =
		(decltype(CoreGetComponentRegistry))GetProcAddress(GetModuleHandle(xorstr_("CoreRT.dll")), xorstr_("CoreGetComponentRegistry"));

	return CoreGetComponentRegistry();
}
} // fx