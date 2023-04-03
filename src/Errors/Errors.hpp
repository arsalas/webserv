#pragma once
#include <iostream>

class Errors : public std::exception
{
private:
	std::string _message;
	int _code;

public:
	Errors(int code, std::string msg);

	virtual const char *what() const throw();
};
