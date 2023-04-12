#include "menus.h"
#include "lua.h"

#include "../../dllmain.h"

#include "../../fx_game/vfs.h"
#include "../../fx_game/resource_manager.h"
#include "../../fx_game/resource.h"
#include "../../fx_game/vfs.h"

#include "../../util/str.h"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_stdlib.h"
#include "../../include/xorstr/xorstr.hpp"

#include <fstream>
#include <filesystem>

namespace ui_menus
{
std::vector<std::string> Resources::m_savedScripts;

std::optional<std::string> Resources::read_resource_file(const std::string& path)
{
	auto stream = fx::Stream::open_read(path);
	if (!stream.GetRef())
		return std::nullopt;

	std::vector<uint8_t> data;
	data.resize(stream->GetLength());

	stream->Read(data.data(), data.size());

	return std::string(data.begin(), data.end());
}

void Resources::save_resource_files(fx::fwRefContainer<fx::Resource> resource, const std::string& saveFolder)
{
	auto metadataComp = resource->get_component<fx::ResourceMetaDataComponent>();
	if (!metadataComp)
		return;

	auto clientScripts = metadataComp->glob_entries_vector(xorstr_("client_script"));
	auto sharedScripts = metadataComp->glob_entries_vector(xorstr_("shared_script"));

	auto files = metadataComp->glob_entries_vector(xorstr_("file"));
	files.push_back(xorstr_("fxmanifest.lua"));
	files.push_back(xorstr_("__resource.lua"));

	for (auto& allFiles : { clientScripts, sharedScripts, files })
	{
		for (auto& filename : allFiles)
		{
			std::string filePath = resource->GetPath() + filename;
			auto fileData = read_resource_file(filePath);

			if (!fileData.has_value())
				continue;

			auto savePath = saveFolder + util::split_str(filePath, ':')[1];

			std::error_code ec;
			std::filesystem::create_directories(std::filesystem::path(savePath).remove_filename(), ec);

			if (!ec)
			{
				std::ofstream file(savePath, std::ios::binary);
				if (file)
					file.write((char*)fileData.value().data(), fileData.value().size());

				m_savedScripts.push_back(savePath);
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

std::optional<std::string> read_file(const std::string& path)
{
	static fx::fwRefContainer<fx::Stream>(*vfs__OpenRead)(const std::string & path) = nullptr;
	if (!vfs__OpenRead)
		vfs__OpenRead = (decltype(vfs__OpenRead))util::get_unhooked_ptr(GetProcAddress(GetModuleHandle(xorstr_("vfs-core.dll")), MAKEINTRESOURCE(102)));

	auto stream = vfs__OpenRead(path);
	if (!stream.GetRef())
		return std::nullopt;

	std::vector<uint8_t> data;
	data.resize(stream->GetLength());

	stream->Read(data.data(), data.size());

	return std::string(data.begin(), data.end());
}

std::vector<std::string> get_resource_events(fx::Resource* resource, bool inc_client_events)
{
	auto component = resource->get_component<fx::ResourceMetaDataComponent>();
	if (!component)
		return {};

	std::vector<std::string> client = component->glob_entries_vector(xorstr_("client_script"));
	std::vector<std::string> shared = component->glob_entries_vector(xorstr_("shared_script"));

	std::vector<std::string> events;
	for (auto& scripts : { client, shared })
	{
		for (auto& script : scripts)
		{
			std::string file_path = resource->GetPath() + script;

			auto script_data = read_file(file_path);
			if (!script_data.has_value())
				continue;

			std::istringstream lines(script_data.value());

			std::string line;
			while (std::getline(lines, line))
			{
				line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

				if (line.find(xorstr_("TriggerServerEvent")) != std::string::npos)
					events.push_back(line);

				if (inc_client_events)
				{
					if (line.find(xorstr_("TriggerEvent")) != std::string::npos)
						events.push_back(line);
				}
			}
		}
	}

	return events;
}

void Resources::draw()
{
	ImGui::SetNextWindowSize({ 500.0f, 300.0f }, ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.90f);

	if (ImGui::Begin(xorstr_("Resources"), nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::PopStyleVar();

		if (ImGui::BeginTabBar(xorstr_("##resources_tabs")))
		{
			if (ImGui::BeginTabItem(xorstr_("Resources")))
			{
				auto resources = fx::ResourceManager::get_current_impl()->get_resources();

				size_t resource_count = resources.size();

				static int curr_resource_idx = -1;
				if (curr_resource_idx > resource_count)
					curr_resource_idx = -1;

				ImGui::BeginGroup();

				static ImGuiTextFilter resource_filter;
				resource_filter.Draw(xorstr_("##resource_filter"), ImGui::GetContentRegionAvailWidth() * 0.5f);
				
				if (ImGui::BeginChild(xorstr_("##resources"), ImVec2{ ImGui::GetContentRegionAvailWidth() * 0.5f, -ImGui::GetFrameHeightWithSpacing() }, true))
				{
					for (int i = 0; i < resources.size(); i++)
					{
						bool is_selected = (curr_resource_idx == i);

						auto resource_name = resources[i]->GetName();
						if (resource_filter.PassFilter(resource_name.c_str()))
						{
							if (resources[i]->get_impl()->m_state == fx::ResourceState::Started)
								resource_name += xorstr_(" (Started)");
							else
								resource_name += xorstr_(" (Stopped)");

							if (ImGui::Selectable(resource_name.c_str(), is_selected))
								curr_resource_idx = i;
						}
						else
						{
							if (is_selected)
								curr_resource_idx = -1;
						}
					}

					ImGui::EndChild();
				}

				if (curr_resource_idx != -1)
				{
					if (ImGui::Button(xorstr_("Start")))
						resources[curr_resource_idx]->get_impl()->m_state = fx::ResourceState::Started;

					ImGui::SameLine();

					if (ImGui::Button(xorstr_("Stop")))
						resources[curr_resource_idx]->get_impl()->m_state = fx::ResourceState::Stopped;

					/*
					ImGui::SameLine();

					if (ImGui::Button("View State"))
						MessageBox(nullptr, std::format("State: {}, Name: {}", (int)resources[curr_resource_idx]->get_impl()->m_state, resources[curr_resource_idx]->get_impl()->m_name.c_str()).c_str(), nullptr, MB_OK);
				
					*/
				}
				else
				{
					ImGui::Text(xorstr_("Please select a resource to Start/Stop."));
				}

				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::TextWrapped(xorstr_("Viewing handled events is currently not supported."));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Files")))
			{
				if (ImGui::BeginTabBar(xorstr_("##dumper_tabs")))
				{
					if (ImGui::BeginTabItem(xorstr_("Dumper")))
					{
						static std::string dumpPath;
						if (dumpPath.empty())
						{
							std::filesystem::path dllPath(g_dllPath);
							dumpPath = (dllPath.remove_filename() / xorstr_("dumps\\my_first_dump")).string();
						}

						if (ImGui::BeginChild(xorstr_("dumper"), ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true))
						{
							if (!m_savedScripts.empty())
							{
								for (auto& script : m_savedScripts)
									ImGui::Text("%s", script.c_str());
							}
							else
							{
								ImGui::TextWrapped(xorstr_("Press Save to dump available client files."));
							}

							ImGui::EndChild();
						}

						if (ImGui::Button(xorstr_("Save")))
						{
							for (auto& resource : fx::ResourceManager::get_current_impl()->get_resources())
								save_resource_files(resource, dumpPath);
						}

						ImGui::SameLine();

						ImGui::PushItemWidth(-1.0f);
						ImGui::InputText(xorstr_("##dumper_folder"), &dumpPath);
						ImGui::PopItemWidth();

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem(xorstr_("Events")))
					{
						static bool incl_client_events = false;

						static std::string event_filter;
						ImGui::PushItemWidth(-1.0f);
						ImGui::InputText(xorstr_("##event_filterrr"), &event_filter);
						ImGui::PopItemWidth();

						static std::map<std::string, std::vector<std::string>> resource_events;

						auto get_filtered_events = []
						{
							std::map<std::string, std::vector<std::string*>> filtered_events;

							for (auto& [resource, events] : resource_events)
							{
								if (!event_filter.empty())
								{
									bool found = false;
									for (auto& event : events)
									{
										if (event.find(event_filter) != std::string::npos)
										{
											found = true;
											break;
										}
									}

									if (!found)
										continue;
								}

								for (auto& event : events)
								{
									if (event.find(event_filter) != std::string::npos)
										filtered_events[resource].push_back(&event);
								}
							}

							return filtered_events;
						};

						if (ImGui::BeginChild(xorstr_("##crevents"), ImVec2{ -1.0, -ImGui::GetFrameHeightWithSpacing() }, true))
						{
							if (!resource_events.empty())
							{
								auto filtered_events = get_filtered_events();
								if (!filtered_events.empty())
								{
									int id = 0;

									for (auto& [resource, events] : filtered_events)
									{
										ImGui::PushID(id);

										if (ImGui::CollapsingHeader(resource.c_str(), ImGuiTreeNodeFlags_None))
										{
											for (int i = 0; i < events.size(); i++)
											{
												ImGui::PushID(i);

												ImGui::PushItemWidth(-55.0f);
												ImGui::InputText(xorstr_("##event_name"), &*events[i]);
												ImGui::PopItemWidth();

												ImGui::SameLine();

												if (ImGui::Button(xorstr_("Trigger")))
													execute(*events[i]);

												ImGui::PopID();
											}
										}

										ImGui::PopID();

										id++;
									}
								}
							}
							else
							{
								ImGui::TextWrapped(xorstr_("Press Scan to scan for events."));
							}

							ImGui::EndChild();
						}

						if (ImGui::Button(xorstr_("Scan")))
						{
							resource_events.clear();

							auto resources = fx::ResourceManager::get_current_impl()->get_resources();
							for (auto& resource : resources)
							{
								auto events = get_resource_events(resource.GetRef(), incl_client_events);
								if (!events.empty())
									resource_events[resource->GetName()] = events;
							}
						}

						ImGui::SameLine();

						if (ImGui::Button(xorstr_("Copy to CB")))
						{
							std::stringstream ss;

							for (auto& [resource, events] : get_filtered_events())
							{
								ss << resource << "\n";
								for (auto& event : events)
									ss << *event << "\n";

								ss << "\n\n";
							}

							ImGui::SetClipboardText(ss.str().c_str());
						}

						ImGui::SameLine();

						ImGui::Checkbox(xorstr_("Include Client Events"), &incl_client_events);

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}
} // ui_menus