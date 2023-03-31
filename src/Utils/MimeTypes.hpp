#pragma once
#include <iostream>
#include <map>

class MimeTypes {
public:
	MimeTypes();
	std::string &operator[](std::string);

private:
	std::map<std::string, std::string> _type;
	std::string _bin;
};

