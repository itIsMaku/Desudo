#pragma once
#include "../fx/event.h"
#include "../fx/component.h"
#include "../fx/omptr.h"

#include "../util/address.h"

#include "../tbb/concurrent_unordered_map.h"
#include "../tbb/concurrent_unordered_set.h"

#include <map>
#include <Windows.h>

namespace fx
{
class ResourceManager;
class ResourceManagerImpl;

class ResourceMetaDataComponent;
class ResourceScriptingComponent;
class ResourceEventManagerComponent;
class ResourceEventComponent;

class IScriptHost;
class IScriptTickRuntime;
class IScriptRuntime;

class ResourceImpl;

enum ResourceState
{
	Uninitialized,
	Stopped,
	Starting,
	Started,
	Stopping
};

class Resource : public fwRefCountable, public ComponentHolderImpl<Resource>
{
public:
	fwEvent<> OnBeforeStart;
	fwEvent<std::vector<char>*> OnBeforeLoadScript;
	fwEvent<> OnStart;
	fwEvent<> OnStop;
	fwEvent<> OnEnter;
	fwEvent<> OnLeave;
	fwEvent<> OnCreate;
	fwEvent<> OnActivate;
	fwEvent<> OnDeactivate;
	fwEvent<> OnRemove;

public:
	virtual const std::string& GetName() = 0;

	virtual const std::string& GetIdentifier() = 0;

	virtual const std::string& GetPath() = 0;

	virtual ResourceState GetState() = 0;

	virtual bool LoadFrom(const std::string& rootPath, std::string* errorResult) = 0;

	virtual bool Start() = 0;

	virtual bool Stop() = 0;

	virtual void Run(std::function<void()>&& func) = 0;

	virtual ResourceManager* GetManager() = 0;

public:
	ResourceImpl* get_impl();

	template<typename T>
	T* get_component()
	{
		if constexpr (std::is_same_v<T, ResourceMetaDataComponent>)
		{
			// fx::ComponentRegistry::GetImpl()->RegisterComponent("fx::ResourceMetaDataComponent");
			return (T*)GetInstanceRegistry()->GetInstance(16).GetRef();
		}
		else if constexpr (std::is_same_v<T, ResourceScriptingComponent>)
		{
			// fx::ComponentRegistry::GetImpl()->RegisterComponent("fx::ResourceScriptingComponent");
			return (T*)GetInstanceRegistry()->GetInstance(22).GetRef();
		}
		else if constexpr (std::is_same_v<T, ResourceEventComponent>)
		{
			// fx::ComponentRegistry::GetImpl()->RegisterComponent("fx::ResourceEventComponent");
			return (T*)GetInstanceRegistry()->GetInstance(14).GetRef();
		}

		return nullptr;
	}

	// IScriptHost* create_scripthost();
};

class ResourceImpl : public Resource
{
public:
	std::string m_name;
	std::string m_rootPath;

	ResourceManagerImpl* m_manager;

	ResourceState m_state;
};

class ResourceEventComponent : public fwRefCountable, public IAttached<Resource>
{
public:
	Resource* m_resource;

	ResourceEventManagerComponent* m_managerComponent;

	fwEvent<const std::string&, const std::string&, const std::string&, bool*> OnTriggerEvent;

public:
	virtual void AttachToObject(Resource* object) = 0;
};

class ResourceScriptingComponent : public fwRefCountable
{
public:
	Resource* m_resource;

	fx::OMPtr<IScriptHost> m_scriptHost;

	tbb::concurrent_unordered_map<int32_t, fx::OMPtr<IScriptRuntime>> m_scriptRuntimes;
	std::unordered_map<int32_t, fx::OMPtr<IScriptTickRuntime>> m_tickRuntimes;

	tbb::concurrent_unordered_set<std::string> m_eventsHandled;

public:
	fwEvent<> OnCreatedRuntimes;
	fwEvent<const std::string&, const std::string&> OnOpenScript;

public:
	template<typename TFunc>
	void for_all_runtimes(TFunc&& func)
	{
		for (auto& runtime : m_scriptRuntimes)
			func(runtime.second);
	}
};

typedef void* ResourceMetaDataLoader;

class ResourceMetaDataComponent : public fwRefCountable
{
public:
	Resource* m_resource;

	std::multimap<std::string, std::string> m_metaDataEntries;

	fwRefContainer<ResourceMetaDataLoader> m_metaDataLoader;

public:
	std::vector<std::string> glob_entries_vector(const std::string& key);
};
} // fx