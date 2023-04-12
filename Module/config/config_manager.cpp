#include "config_manager.h"

#include "../dllmain.h"

#include "../include/xorstr/xorstr.hpp"

#include <format>
#include <filesystem>

namespace config
{
File& Manager::get_config(const std::string& configName)
{
	if (m_configCache.find(configName) == m_configCache.end())
		m_configCache[configName] = File{ configName };

	return m_configCache[configName];
}

config::Manager& Manager::get_instance()
{
	static Manager instance;
	return instance;
}

std::string Manager::get_config_folder()
{
	if (m_configsFolderPath.empty())
	{
		std::filesystem::path dllPath(g_dllPath);
		m_configsFolderPath = (dllPath.remove_filename() / xorstr_("configs")).string();
	}

	return m_configsFolderPath;
}
} // config