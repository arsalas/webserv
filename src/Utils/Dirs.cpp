#include <iostream>
#include <fstream>
#include <vector>
#include "Dirs.hpp"
#include "Strings.hpp"

bool Dirs::existDir(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file.good())
		return false;
	file.close();
	return true;
}

std::string Dirs::getFilenameFromPath(std::string path)
{
	std::vector<std::string> parts = Strings::split(path, "/");
	return parts[parts.size() - 1];
}