#include "runtime_manager.h"

#include "../include/xorstr/xorstr.hpp"

#include "../fx_game/resource.h"
#include "../fx_game/resource_manager.h"

namespace scripting
{
fx::LuaScriptRuntime* Manager::create_runtime(fx::fwRefContainer<fx::Resource> resource)
{
	guid_t guid = { 0x567634c6, 0x3bdd, 0x4d0e, { 0xaf, 0x39, 0x74, 0x72, 0xae, 0xd4, 0x79, 0xb7 } };

	guid_t clsid;
	intptr_t findHandle = fx::findFirstImpl(guid, &clsid);
	if (findHandle != 0)
	{
		int currentIdx = 0;
		do
		{
			if (currentIdx == 0)
			{
				fx::findImplClose(findHandle);

				// create the base runtime.
				fx::IScriptRuntime* runtime = nullptr;
				if (fx::createObjectInstance(clsid, guid, (void**)&runtime) == 0)
				{
					// get the latest natives in the runtime.
					auto metadataComp = resource->get_component<fx::ResourceMetaDataComponent>();
					if (metadataComp)
						metadataComp->m_metaDataEntries.insert({ xorstr_("fx_version"), xorstr_("adamant") });

					// set the runtime's parent resource.
					auto runtimeImpl = runtime->get_impl<fx::LuaScriptRuntime>();
					runtimeImpl->SetParentObject(resource.GetRef());

					// create the runtime with the parent resource's own scripthost.
					auto scripting = resource->get_component<fx::ResourceScriptingComponent>();
					runtimeImpl->Create(scripting->m_scriptHost.GetRef());

					// destroy the runtime and remove it from our list when the parent resource is destroyed.
					resource->OnRemove.Connect([=]
					{
						runtimeImpl->Destroy();
						m_runtimes.erase(std::remove(m_runtimes.begin(), m_runtimes.end(), runtimeImpl), m_runtimes.end());
					});
					
					// clear the native metadata if it was added.
					if (metadataComp)
					{
						if (metadataComp->m_metaDataEntries.count(xorstr_("fx_version")) == 1)
							metadataComp->m_metaDataEntries.erase(xorstr_("fx_version"));
					}
					
					m_runtimes.push_back(runtime);

					return runtimeImpl;
				}

				break;
			}

			currentIdx++;
		} while (fx::findNextImpl(findHandle, &clsid));

		fx::findImplClose(findHandle);
	}

	return nullptr;
}

std::vector<void*>& Manager::get_runtimes()
{
	return m_runtimes;
}

scripting::Manager& Manager::get_instance()
{
	static Manager instance;
	return instance;
}
} // scripting