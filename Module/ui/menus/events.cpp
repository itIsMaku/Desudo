#include "menus.h"

#include "../../fx_game/resource_manager.h"
#include "../../fx_game/netlibrary.h"

#include "../../nlohmann/json.hpp"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_stdlib.h"

#include "../../include/xorstr/xorstr.hpp"

namespace ui_menus
{
bool Events::m_blockEvents = false;

std::vector<Events::EventLog> Events::m_serverLog;
std::vector<Events::EventLog> Events::m_clientLog;

std::map<std::string, bool> Events::m_blockedEvents;

void Events::draw_server_tab()
{
	static ImGuiTextFilter name_filter;
	name_filter.Draw(xorstr_("##cevent_name_filter"), ImGui::GetContentRegionAvailWidth() * 0.4f);

	ImGui::SameLine();

	static ImGuiTextFilter data_filter;
	data_filter.Draw(xorstr_("##cevent_data_filter"), -1.0f);

	if (ImGui::BeginChild(xorstr_("##server_events"), ImVec2{ -1.0f, -ImGui::GetFrameHeightWithSpacing() }, true))
	{
		if (!m_serverLog.empty())
		{
			for (int i = 0; i < m_serverLog.size(); i++)
			{
				auto& event = m_serverLog[i];

				if (name_filter.PassFilter(event.name.c_str()) &&
					data_filter.PassFilter(event.args.c_str()))
				{
					ImGui::PushID(i);

					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					ImGui::InputText(xorstr_("##event_name"), &event.name);
					ImGui::PopItemWidth();

					ImGui::SameLine();

					ImGui::PushItemWidth(-1.0f);
					ImGui::InputText(xorstr_("##event_args"), &event.args);
					ImGui::PopItemWidth();

					ImGui::PopID();
				}
			}
		}
		else
		{
			ImGui::TextWrapped(xorstr_("The client has not triggered any server events."));
		}

		static bool enable_autoscroll = true;
		if (enable_autoscroll)
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();

		if (ImGui::Button(xorstr_("Clear")))
			m_serverLog.clear();

		ImGui::SameLine();

		ImGui::Checkbox(xorstr_("Auto-scroll"), &enable_autoscroll);
	}
}

void Events::draw_client_tab()
{
	static bool enable_autoscroll = true;

	static ImGuiTextFilter name_filter;
	name_filter.Draw(xorstr_("##cevent_name_filter"), ImGui::GetContentRegionAvailWidth() * 0.4f);

	ImGui::SameLine();

	static ImGuiTextFilter data_filter;
	data_filter.Draw(xorstr_("##cevent_data_filter"), -1.0f);

	if (ImGui::BeginChild(xorstr_("##cevents"), ImVec2{ -1.0, -ImGui::GetFrameHeightWithSpacing() }, true))
	{
		if (!m_clientLog.empty())
		{
			for (auto i = 0; i < m_clientLog.size(); i++)
			{
				auto& event = m_clientLog[i];

				if (name_filter.PassFilter(event.name.c_str()) &&
					data_filter.PassFilter(event.args.c_str()))
				{
					ImGui::PushID(i);

					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					ImGui::InputText(xorstr_("##event_name"), &event.name);
					ImGui::PopItemWidth();

					ImGui::SameLine();

					ImGui::PushItemWidth(-1.0f);
					ImGui::InputText(xorstr_("##event_args"), &event.args);
					ImGui::PopItemWidth();

					ImGui::PopID();
				}
			}
		}
		else
		{
			ImGui::TextWrapped(xorstr_("The server/client has not triggered any client events."));
		}

		if (enable_autoscroll)
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}

	if (ImGui::Button(xorstr_("Clear")))
		m_clientLog.clear();

	ImGui::SameLine();

	ImGui::Checkbox("Auto-scroll", &enable_autoscroll);
}

void Events::draw_blocker_tab()
{
	if (ImGui::BeginChild(xorstr_("##bevents"), ImVec2{ -1.0, -ImGui::GetFrameHeightWithSpacing() }, true))
	{
		if (m_blockEvents)
			ImGui::Text(xorstr_("Block All Events is enabled in Settings."));

		if (!m_blockedEvents.empty())
		{
			ImGui::TextWrapped(xorstr_("Blocked events will still show in the Server/Client tab when they are blocked."));

			for (auto& [event, is_filter] : m_blockedEvents)
			{
				auto label = (is_filter ? event + xorstr_(" (Filter)") : event);

				if (ImGui::Button(label.c_str(), { -1.0f, 0.0f }))
					m_blockedEvents.erase(event);
			}
		}
		else
		{
			if (!m_blockEvents)
				ImGui::TextWrapped(xorstr_("You are not blocking any events."));
		}

		ImGui::EndChild();
	}

	static bool use_filter = false;
	static std::string event_input = xorstr_("ban_me");

	ImGui::PushItemWidth(-140.0f);
	ImGui::InputText("##blocked_event_input", &event_input);
	ImGui::PopItemWidth();

	ImGui::SameLine();

	ImGui::Checkbox("Use Filter", &use_filter);

	ImGui::SameLine();

	if (ImGui::Button(xorstr_("Block")))
		m_blockedEvents[event_input] = use_filter;
}

void Events::draw()
{	
	static std::once_flag eventsInitialized;
	std::call_once(eventsInitialized, []
	{
		auto resource = fx::ResourceManager::get_current_impl()->get_resource(xorstr_("_cfx_internal"));
		if (resource.GetRef())
		{
			auto component = resource->get_component<fx::ResourceEventComponent>();
			if (component)
			{
				component->OnTriggerEvent.Connect([](const std::string& eventName, const std::string& eventPayload, const std::string& eventSource, bool* eventCanceled)
				{
					EventLog log;
					log.name = eventName;

					auto str = nlohmann::json::from_msgpack(eventPayload.begin(), eventPayload.end(), true, false);
					if (!str.is_discarded())
						log.args = str.dump();
					else
						log.args = "Failed to parse arguments.";

					if (m_clientLog.size() > 20000)
						m_clientLog.clear();

					m_clientLog.push_back(log);
				}, INT32_MIN);
			}
		}
		 
		fx::NetLibrary* netLibrary = *(fx::NetLibrary**)((uint64_t)GetModuleHandle(xorstr_("gta-net-five.dll")) + 0x221440);
		netLibrary->OnTriggerServerEvent.Connect([](std::string& name, std::string& payload)
		{
			EventLog log;
			log.name = name;

			auto str = nlohmann::json::from_msgpack(payload.begin(), payload.end(), true, false);
			if (!str.is_discarded())
				log.args = str.dump();
			else
				log.args = "Failed to parse arguments.";

			if (m_serverLog.size() > 20000)
				m_serverLog.clear();

			m_serverLog.push_back(log);

			if (!m_blockEvents)
			{
				for (auto& [event, isFilter] : m_blockedEvents)
				{
					if (isFilter)
					{
						if (name.find(event) != std::string::npos)
							name = xorstr_("???1???1");
					}
					else
					{
						if (event == name)
							name = xorstr_("??11?1?");
					}
				}
			}
			else
			{
				name = xorstr_("????????22");
			}
		}, INT32_MIN);
	});

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.90f);

	ImGui::SetNextWindowSize({ 500.0f, 300.0f }, ImGuiCond_Once);

	if (ImGui::Begin(xorstr_("Events"), nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::PopStyleVar();

		if (ImGui::BeginTabBar(xorstr_("##events")))
		{
			if (ImGui::BeginTabItem(xorstr_("Server")))
			{
				draw_server_tab();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Client")))
			{
				draw_client_tab();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Blocker")))
			{
				draw_blocker_tab();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Settings")))
			{
				ImGui::Checkbox(xorstr_("Block All Events"), &m_blockEvents);
				ImGui::EndTabItem();
			}
		}

		ImGui::End();
	}
}
} // ui_menus