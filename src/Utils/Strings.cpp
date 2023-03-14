#include "Strings.hpp"
#include <sstream>

const std::string Strings::intToString(int number)
{
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

std::string Strings::ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t");
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Strings::rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(" \t");
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Strings::trim(const std::string &s)
{
	return (Strings::rtrim(Strings::ltrim(s)));
}

std::vector<std::string>    Strings::vectorSplit(std::string file, std::string delimiter)
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