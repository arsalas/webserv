#pragma once

#include <iostream>
#include <vector>

class Strings
{
public:
	static const std::string intToString(int);
	static std::string trim(const std::string &s);
	static std::string trim(const std::string &s, const std::string &delim);
	static std::vector<std::string> vectorSplit(std::string file, std::string delimiter);
	static std::string deleteQuotes(std::string str);
	static std::string rtrim(const std::string &s, const std::string &delim);
	static std::string ltrim(const std::string &s, const std::string &delim);

private:
	Strings();
};
