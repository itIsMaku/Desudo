#include "menus.h"
#include "lua.h"

#include "../../fx_game/resource_manager.h"
#include "../../fx_game/scripthost.h"

#include "../../fx/component.h"

#include "../../scripting/runtime_manager.h"

#include "../../ui/hook/d3d11_hook.h"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_stdlib.h"
#include "../../include/xorstr/xorstr.hpp"

namespace ui_menus
{
void execute(const std::string& script)
{
	std::vector<fx::fwRefContainer<fx::Resource>> validResources;

	fx::ResourceManager::get_current_impl()->for_all_resources([&validResources](fx::fwRefContainer<fx::Resource> resource)
	{
		auto scriptingComp = resource->get_component<fx::ResourceScriptingComponent>();
		if (scriptingComp)
		{
			if (!scriptingComp->m_scriptHost.GetRef())
				return;

			validResources.push_back(resource);
		}
	});

	if (validResources.empty())
		return;

	auto resource = validResources[rand() % validResources.size()];

	auto runtime = scripting::Manager::get_instance().create_runtime(validResources[rand() % validResources.size()]);
	runtime->execute_script(script, xorstr_("@nk.lua"));
}

void LuaExecutor::draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.90f);

	ImGui::SetNextWindowSize({ 500.0f, 300.0f }, ImGuiCond_Once);

	if (ImGui::Begin(xorstr_("Lua Executor"), nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::PopStyleVar();

		static std::string scriptInput = xorstr_("print('Hello World!')");

		ImGui::PushItemWidth(-1.0);
		ImGui::InputTextMultiline(xorstr_("##lua_input"), &scriptInput, ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), ImGuiInputTextFlags_AllowTabInput);

		if (ImGui::Button(xorstr_("Execute")))
			execute(scriptInput);

		ImGui::End();
	}
}
} // ui_menus