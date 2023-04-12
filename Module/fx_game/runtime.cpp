#include "runtime.h"
#include "scripthost.h"
#include "resource.h"

#include "../include/xorstr/xorstr.hpp"

#include "../scripting/vfs_hook.h"

void fx::LuaScriptRuntime::execute_script(const std::string& script, const std::string& scriptName)
{
	auto resource = (Resource*)GetParentObject();
	resource->OnBeforeLoadScript.Reset();
	resource->get_component<fx::ResourceScriptingComponent>()->OnOpenScript.Reset();

	scripting::scriptToExecute = script;

	uint64_t* scripthost_vt = *(uint64_t**)m_scriptHost;

	uint64_t vt_copy[32] = { 0 };
	memcpy(vt_copy, scripthost_vt, sizeof(vt_copy));

	vt_copy[5] = (uint64_t)IScriptHost::OpenHostFile_hook;

	uint64_t old_vt = *(uint64_t*)m_scriptHost;
	*(uint64_t*)m_scriptHost = (uint64_t)vt_copy;
	
	LoadFile((char*)scriptName.c_str());

	*(uint64_t*)m_scriptHost = old_vt;
}