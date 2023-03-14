#include <unistd.h>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

int main()
{

	DIR *dir_fd = opendir("www");
	struct dirent *dir_file;
	while ((dir_file = readdir(dir_fd)))
	{
		std::cout << "" << dir_file->d_name << std::endl;
		// std::cout << "type: " << dir_file->d_type << std::endl;
	}
	closedir(dir_fd);
	return 0;
}
