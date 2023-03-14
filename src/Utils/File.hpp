#pragma once

#include <iostream>
#include <fstream>

class File
{
private:
	// int _fd;
	std::string _path;
	std::ifstream _file;

private:
	File();

public:
	File(std::string path);
	~File();

	std::string toStr();

	// EXCEPTIONS
public:
	class NotFoundException : public std::exception
	{
		virtual const char *what() const throw();
	};
};