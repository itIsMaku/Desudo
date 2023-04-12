#include "resource_manager.h"
#include "resource.h"

#include "../util/address.h"

#include "../include/xorstr/xorstr.hpp"

#include <Windows.h>

namespace fx
{
ResourceManager* ResourceManager::get_current(bool allowFallback)
{
	static ResourceManager* (*GetCurrent)(bool allowFallback) =
		(decltype(GetCurrent))util::get_unhooked_ptr(GetProcAddress(GetModuleHandle(xorstr_("citizen-resources-core.dll")), (LPCSTR)MAKEINTRESOURCE(72)));
	
	return GetCurrent(allowFallback);
}

ResourceManagerImpl* ResourceManager::get_current_impl(bool allowFallback)
{
	return (ResourceManagerImpl*)&((uint64_t*)get_current(allowFallback))[2];
}
 
fwRefContainer<Resource> ResourceManagerImpl::get_resource(const std::string& name)
{
	std::unique_lock<std::recursive_mutex> lock(m_resourcesMutex);

	fwRefContainer<Resource> resource;

	auto it = m_resources.find(name);
	if (it != m_resources.end())
		resource = it->second;

	return resource;
}

std::vector<fwRefContainer<Resource>> ResourceManagerImpl::get_resources()
{
	std::vector<fwRefContainer<Resource>> resources;

	for (auto& resource : m_resources)
		resources.push_back(resource.second);

	return resources;
}
} // fx