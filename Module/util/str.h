#pragma once
#include <vector>
#include <iostream>
#include <sstream>

namespace util
{
	inline std::vector<std::string> split_str(const std::string& str, char delim)
	{
		std::vector<std::string> results;

		std::string item;

		std::stringstream ss(str);
		while (std::getline(ss, item, delim))
			results.push_back(item);

		return results;
	}
}