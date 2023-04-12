#include "resource.h"

#include "../include/xorstr/xorstr.hpp"

#include <format>

namespace fx
{
ResourceImpl* Resource::get_impl()
{
	return (ResourceImpl*)&((uint64_t*)this)[0];
}

std::vector<std::string> ResourceMetaDataComponent::glob_entries_vector(const std::string& key)
{
	static void(*GlobEntriesVector)(ResourceMetaDataComponent * _this, std::vector<std::string>& out, const std::string & key) =
		(decltype(GlobEntriesVector))util::get_unhooked_ptr(GetProcAddress(GetModuleHandle(xorstr_("citizen-resources-core.dll")), (LPCSTR)MAKEINTRESOURCE(78)));

	std::vector<std::string> ret;
	GlobEntriesVector(this, ret, key);

	return ret;
}
} // fx