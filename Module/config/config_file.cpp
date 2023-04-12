#include "config_file.h"
#include "config_manager.h"

#include "../util/file.h"

#include <format>
#include <Windows.h>
#include "../include/xorstr/xorstr.hpp"

namespace config
{
File::File(const std::string& configName)
	: m_configName(configName)
{
	std::string configData = util::read_file(std::format(("{}\\{}.json"), Manager::get_instance().get_config_folder(), configName));
	if (!configData.empty())
	{
		m_configJson = nlohmann::json::parse(configData, nullptr, false);
		if (m_configJson.is_discarded())
		{
			MessageBox(nullptr, std::format(("Failed to parse config file: {}.json. Falling back to defaults."), configName).c_str(), ("Error"), MB_OK);
			m_configJson = {};
		}
	}
}

bool File::save(bool showError)
{
	bool saveSuccess = util::save_file(std::format(("{}\\{}.json"), Manager::get_instance().get_config_folder(), m_configName), m_configJson.dump(4));
	if (!saveSuccess && showError)
		MessageBox(nullptr, std::format(("Failed to save config file: {}.json."), m_configName).c_str(), ("Error"), MB_OK);

	return saveSuccess;
}
} // config