#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include <algorithm> // std::sort
#include "Autoindex.hpp"
#include "Utils/Strings.hpp"

Autoindex::Autoindex() : APage(), _path(""), _root("")
{
}

Autoindex::Autoindex(std::string path, std::string root) : APage(), _path(path), _root(root)
{
}

Autoindex::~Autoindex()
{
}

std::string Autoindex::toStr() const
{
	std::ostringstream ss;

	ss << getHeader() << getBody() << getFooter();
	return ss.str();
}

std::vector<std::string> Autoindex::readDir() const
{
	std::ostringstream ss;
	std::vector<std::string> dirs;
	std::cout << _path.c_str();
	DIR *dirFd = opendir(_path.c_str());
	if (!dirFd)
	{
		std::cout << "2\n";
		throw NotFoundException(); 
	}

	struct dirent *dirFile;
	while ((dirFile = readdir(dirFd)))
	{
		dirs.push_back(std::string(dirFile->d_name));
	}
	closedir(dirFd);
	std::sort(dirs.begin(), dirs.end());
	return dirs;
}

std::string Autoindex::getHeader() const
{
	std::ostringstream ss;

	ss << "<!DOCTYPE html>" << std::endl;
	ss << "<html lang=\"en\">" << std::endl;
	ss << "<head>" << std::endl;
	ss << "<meta charset=\"UTF-8\">" << std::endl;
	ss << "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	ss << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	ss << "<title>Index of " << _path << "</title>" << std::endl;
	ss << "</head>" << std::endl;
	return ss.str();
}

std::string Autoindex::getBody() const
{
	std::vector<std::string> dirs = readDir();
	std::ostringstream ss;
	ss << "<body>" << std::endl;
	ss << "<h1>Index of " << _path << "</h1>" << std::endl;
	ss << "<hr>" << std::endl;
	for (unsigned long i = 0; i < dirs.size(); i++)
	{
		if (dirs[i] == ".")
			ss << "<div><a href=\"/" << _root << "\">" << dirs[i] << "</a></div>" << std::endl;
		else if (dirs[i] == "..")
		{
			if (_root == "")
				continue;
			std::vector<std::string> parts = Strings::split(_root, "/");
			std::string root = "";
			for (size_t i = 0; i < parts.size() - 1; i++)
			{
				root += parts[i];
			}
			ss << "<div><a href=\"/" << root << "\">" << dirs[i] << "</a></div>" << std::endl;
		}
		else
			ss << "<div><a href=\"/" << _root << "/" << dirs[i] << "\">" << dirs[i] << "</a></div>" << std::endl;
	}
	ss << "<hr>" << std::endl;
	ss << "</body>" << std::endl;
	return ss.str();
}

std::string Autoindex::getFooter() const
{
	std::ostringstream ss;

	ss << "</html>" << std::endl;
	return ss.str();
}


const char *Autoindex::NotFoundException::what() const throw()
{
	return "Not found";
}
