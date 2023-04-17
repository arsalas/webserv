#include "File.hpp"
#include <string>
#include "Utils/Strings.hpp"

File::File() {}
File::File(std::string path) : _path(path)
{
	_file.open(_path.c_str());
	if (!_file.is_open())
	{
		throw NotFoundException();
	}
}

File::~File()
{
	_file.close();
}

std::string File::getExecPath(std::string path)
{
    int i = path.find_last_not_of('/');
    std::string str = path.substr(i);
    return (str);
}

std::string File::toStr()
{
	std::string line;
	std::string str;
	if (_file.is_open())
	{
		while (std::getline(_file, line))
		{
			str += line;
			str += "\n";
		}
	}
	return str;
}

void File::move(std::string path)
{
	std::ofstream newFile(path, std::ios::binary);
	newFile << toStr();
	newFile.close();
	_file.clear();
}

std::string File::getExtension()
{
	return getExtension(_path);
}

std::string File::getExtension(std::string filename)
{
	std::vector<std::string> parts;
	parts = Strings::split(filename, ".");
	if (parts.size() == 1)
		return "";
	return parts[parts.size() - 1];
}

// Exceptions

const char *File::NotFoundException::what() const throw()
{
	return "File not found";
}
