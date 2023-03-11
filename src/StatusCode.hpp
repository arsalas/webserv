#pragma once

#include <map>
#include <string>

class StatusCode
{
public:
	StatusCode();
	std::string &operator[](int status_code);

private:
	std::map<int, std::string> error_codes_;
};
