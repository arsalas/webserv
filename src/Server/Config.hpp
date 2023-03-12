#pragma once
#include <iostream>
#include <vector>
#include <map>

class Config
{
private:
	std::vector<int> _listen;
	std::vector<std::string> _serverName;
	std::string _root;
	std::map<std::string, std::string> _cgi;
	std::string _index;
	std::map<std::string, Config> _location;
	std::vector<std::string> _limitExcept;

public:
	Config();
};
