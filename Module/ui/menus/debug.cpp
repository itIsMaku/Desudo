#include "menus.h"

#include "../../scripting/runtime_manager.h"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_stdlib.h"
#include "../../include/xorstr/xorstr.hpp"

namespace ui_menus
{
void Debug::draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.90f);

	ImGui::SetNextWindowSize({ 500.0f, 300.0f }, ImGuiCond_Once);

	if (ImGui::Begin(xorstr_("Debug"), nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::PopStyleVar();

		for (auto& runtime : scripting::Manager::get_instance().get_runtimes())
			ImGui::Text("%p", runtime);

		ImGui::End();
	}
}
} // ui_menus