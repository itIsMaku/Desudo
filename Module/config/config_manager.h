#pragma once
#include "config_file.h"

#include <map>

namespace config
{
class Manager
{
private:
	std::string m_configsFolderPath;

	std::map<std::string, File> m_configCache;

public:
	static Manager& get_instance();

	File& get_config(const std::string& configName);

	std::string get_config_folder();
};
} // config