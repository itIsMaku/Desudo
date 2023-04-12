#pragma once
#include "../fx/ref_container.h"
#include "../fx/component.h"
#include "../fx/event.h"

#include <functional>
#include <mutex>
#include <unordered_map>

namespace fx
{
class Resource;
class ResourceImpl;
class ResourceMounter;
class ResourceManagerImpl;

class ResourceManager : public fwRefCountable, public ComponentHolderImpl<ResourceManager>
{
public:
	fwEvent<> OnTick;
	fwEvent<> OnAfterReset;

public:
	virtual void*/*pplx::task<fwRefContainer<Resource>>*/ AddResource(const std::string& uri) = 0;

	virtual void*/*pplx::task<tl::expected<fwRefContainer<Resource>, ResourceManagerError>>*/ AddResourceWithError(const std::string& uri) = 0;

	virtual fwRefContainer<ResourceMounter> GetMounterForUri(const std::string& uri) = 0;

	// Use the get_resource function of ResourceManagerImpl instead.
	virtual fwRefContainer<Resource> _GetResource(const std::string& identifier, bool withProvides = true) = 0;

	// Use the for_all_resources function of ResourceManagerImpl instead.
	virtual void _ForAllResources(const std::function<void(const fwRefContainer<Resource>&)>& function) = 0;

	virtual void ResetResources() = 0;

	virtual void AddMounter(fwRefContainer<ResourceMounter> mounter) = 0;

	virtual void RemoveResource(fwRefContainer<Resource> resource) = 0;

	virtual fwRefContainer<Resource> CreateResource(const std::string& resourceName, const fwRefContainer<ResourceMounter>& mounter) = 0;

	virtual void Tick() = 0;

	virtual void MakeCurrent() = 0;

	virtual std::string CallReferenceInternal(const std::string& functionReference, const std::string& argsSerialized) = 0;

public:
	static ResourceManager* get_current(bool allowFallback = true);

	static ResourceManagerImpl* get_current_impl(bool allowFallback = true);
};

class ResourceManagerImpl : private ResourceManager
{
public:
	std::recursive_mutex m_resourcesMutex;

	std::unordered_map<std::string, fwRefContainer<ResourceImpl>> m_resources;

	std::unordered_multimap<std::string, fwRefContainer<Resource>> m_resourceProvides;

	std::recursive_mutex m_mountersMutex;

	std::vector<fwRefContainer<ResourceMounter>> m_mounters;

public:
	fwRefContainer<Resource> get_resource(const std::string& name);

	template<typename TFunc>
	void for_all_resources(TFunc&& func)
	{
		std::unique_lock<std::recursive_mutex> lock(m_resourcesMutex);

		for (auto& resource : m_resources)
			func(resource.second);
	}

	std::vector<fwRefContainer<Resource>> get_resources();
};
} // fx