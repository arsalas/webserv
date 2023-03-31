#pragma once

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

class Dirs
{
public:
	static bool existDir(std::string);
	static std::string getFilenameFromPath(std::string);

private:
	Dirs();
};
