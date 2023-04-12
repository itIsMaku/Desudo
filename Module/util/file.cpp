#include "file.h"

#include <fstream>
#include <sstream>

std::string util::read_file(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (file)
    {
        std::stringstream ss;
        ss << file.rdbuf();

        return ss.str();
    }

    return {};
}

bool util::save_file(const std::string& filePath, const std::string& data)
{
    std::ofstream file(filePath, std::ios::binary);
    if (file)
    {
        file << data;
        return true;
    }

    return false;
}