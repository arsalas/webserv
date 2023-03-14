#include "File.hpp"
#include <string>

File::File() {}
File::File(std::string path) : _path(path)
{
	_file.open(_path.c_str());
	if (!_file.is_open())
		throw NotFoundException();
}

File::~File()
{
	_file.close();
}

std::string File::toStr()
{
	std::string line;
	std::string str;
	if (_file.is_open())
	{
		while (_file)
		{
			std::getline(_file, line);
			str += line;
			str += "\n";
		}
	}
	return str;
}

// Exceptions

const char *File::NotFoundException::what() const throw()
{
	return "File not found";
}
