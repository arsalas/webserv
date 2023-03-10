#include "Utils.hpp"

std::string Utils::ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t");
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Utils::rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(" \t");
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Utils::trim(const std::string &s)
{
	return (Utils::rtrim(Utils::ltrim(s)));
}

/**
 * @brief SPLIT
 * Find returns the position of the first occurrence of str in the string, or npos if the string is not found
 * Substr creates a new string from the start position until the end ones
 * 
 * @param str 
 * @param del 
 * @return std::string 
 */
std::string Utils::split(std::string str, std::string del)
{
    std::string token = str.substr(0, str.find(del));
    return (token);
}

std::vector<std::string>    Utils::vectorSplit(std::string file, std::string delimiter)
{
    std::vector<std::string> auxVector;
    std::vector<std::string>::iterator  it = auxVector.begin();
    std::string aux;
    size_t pos = 0;
    while ((pos = file.find(delimiter)) != std::string::npos)
    {
        aux = file.substr(0, pos);
        it = auxVector.end();
        auxVector.insert(it, aux);
        file.erase(0, pos + delimiter.length());
    }
    aux = file.substr(0, pos);
    it = auxVector.end();
    auxVector.insert(it, aux);
    return (auxVector);
}
