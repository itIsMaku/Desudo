#pragma once
#include "../nlohmann/json.hpp"

namespace config
{
class File
{
private:
	std::string m_configName;

	nlohmann::json m_configJson;

public:
	File() = default;

	File(const std::string& configName);

	template<typename T>
	nlohmann::json get_key(const std::string& configKey, T defaultVal = {})
	{
		if (m_configJson[configKey].is_null())
		{
			m_configJson[configKey] = defaultVal;
			return defaultVal;
		}

		return m_configJson[configKey].get<T>();
	}

	template<typename T>
	void set_key(const std::string& configKey, T value = {})
	{
		m_configJson[configKey] = value;
	}

	bool save(bool showError = true);
};
} // config