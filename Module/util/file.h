#pragma once
#include <string>

namespace util
{
	std::string read_file(const std::string& filePath);

	bool save_file(const std::string& filePath, const std::string& data);
}