// https://github.com/citizenfx/fivem/blob/master/code/client/shared/ComponentHolder.h
#pragma once
#include "ref_container.h"

#include <map>
#include <string>
#include <vector>

namespace fx
{
	template<typename THolder>
	class IAttached
	{
	public:
		virtual void AttachToObject(THolder* object) = 0;
	};

	template<typename TContained>
	class InstanceRegistryBase : public fwRefCountable
	{
	private:
		std::vector<TContained> m_instances;

	public:
		const TContained& GetInstance(size_t key)
		{
			return m_instances[key];
		}

		std::vector<TContained>& GetAllInstances()
		{
			return m_instances;
		}
	};

	using InstanceRegistry = InstanceRegistryBase<void*>;
	using RefInstanceRegistry = InstanceRegistryBase<fwRefContainer<fwRefCountable>>;

	template<typename THolder>
	class ComponentHolder
	{
	public:
		virtual const fwRefContainer<RefInstanceRegistry>& GetInstanceRegistry() = 0;
	};
	
	template<typename TBase>
	class ComponentHolderImpl : public ComponentHolder<TBase>
	{
	private:
		struct RefInstanceRegistryHolder
		{
			fwRefContainer<RefInstanceRegistry> registry;
		};

		RefInstanceRegistryHolder m_instanceRegistry;
	};

	class ComponentRegistryImpl;

	class ComponentRegistry
	{
	public:
		virtual size_t GetSize() = 0;

		virtual size_t RegisterComponent(const char* key) = 0;

	public:
		static ComponentRegistryImpl* get_impl();
	};
		
	class ComponentRegistryImpl : public ComponentRegistry
	{
	public:
		std::map<std::string, size_t> m_components;

		std::atomic<size_t> m_size;
	};
}