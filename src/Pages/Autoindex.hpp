#pragma once

#include <iostream>
#include <vector>
#include "APage.hpp"

class Autoindex : public APage
{

private:
	std::string _path;
	std::string _root;

public:
	Autoindex();
	Autoindex(std::string path, std::string root);
	virtual ~Autoindex();

	virtual std::string toStr() const;

private:
	std::vector<std::string> readDir() const;
	std::string getHeader() const;
	std::string getBody() const;
	std::string getFooter() const;

	// EXCEPTIONS
	class NotFoundException : public std::exception
	{
		virtual const char *what() const throw();
	};
};
