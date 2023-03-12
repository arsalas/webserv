#pragma once
#include <iostream>
#include <map>

class StatusCode
{
public:
	StatusCode();
	std::string &operator[](int code);

private:
	std::map<int, std::string> _codes;
};
