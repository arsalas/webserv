#include <iostream>
#include "Server/WebServer.hpp"
#include "Http/Request.hpp"
#include "Config/ConfigFile.hpp"

int main(int argc, char **argv)
{
	if (argc == 1)
		WebServer server = WebServer();
	if (argc == 2)
		WebServer server = WebServer(std::string(argv[1]));
	else
		std::cerr << "ERROR\n";
	return 0;
}
