#include "Strings.hpp"
#include <sstream>

const std::string Strings::intToString(int number)
{
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

std::string Strings::ltrim(const std::string &s, const std::string &delim)
{
	size_t start = s.find_first_not_of(delim);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Strings::rtrim(const std::string &s, const std::string &delim)
{
	size_t end = s.find_last_not_of(delim);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Strings::trim(const std::string &s)
{
	const std::string delim = " \t";
	return (Strings::rtrim(Strings::ltrim(s, delim),delim));
}

std::string Strings::trim(const std::string &s, const std::string &delim)
{
	return (Strings::rtrim(Strings::ltrim(s, delim),delim));
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

std::string    Strings::deleteQuotes(std::string str)
{
    int j = 0;
    std::string newStr = "";

    if (str.find("\"") != std::string::npos || str.find("'") != std::string::npos)
    {
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str[i] != '"' && str[i] != '\'')
            {
                newStr.push_back(str[i]);
                j++;
            }
        }
    }
    return (newStr);
}
