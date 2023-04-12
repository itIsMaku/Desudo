#pragma once
#include "../../fx/ref_container.h"

#include "../../fx_game/resource.h"

#include <vector>
#include <string>
#include <optional>

namespace ui_menus
{
	class Resources
	{
	private:
		static std::vector<std::string> m_savedScripts;

	private:
		static std::optional<std::string> read_resource_file(const std::string& path);

		static void save_resource_files(fx::fwRefContainer<fx::Resource> resource, const std::string& saveFolder);

	public:
		static void draw();
	};

	class Events
	{
	private:
		struct EventLog
		{
			std::string name;
			std::string args;
		};

	private:
		static bool m_blockEvents;

		static std::vector<EventLog> m_serverLog;
		static std::vector<EventLog> m_clientLog;

		static std::map<std::string, bool> m_blockedEvents;

	public:
		static void draw();

		static void draw_server_tab();
		static void draw_blocker_tab();
		static void draw_client_tab();
	};

	class LuaExecutor
	{
	public:
		static void draw();
	};

	class Debug
	{
	public:
		static void draw();
	};
} // ui_menus