#pragma once
#include "../fx/interface.h"
#include "../fx/ref_container.h"

#include "../fx_game/runtime.h"
#include "../fx_game/resource.h"

#include <vector>

namespace scripting
{
class Manager
{
private:
	// This feels pointless. Remove it?
	std::vector<void*> m_runtimes;

public:
	static Manager& get_instance();

	// Creates a runtime bound to an existing resource.
	fx::LuaScriptRuntime* create_runtime(fx::fwRefContainer<fx::Resource> resource);

	std::vector<void*>& get_runtimes();
};
} // scripting