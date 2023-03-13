#include <arpa/inet.h>
#include "Server.hpp"

Server::Server() {}
Server::Server(Config config) : _config(config)
{
	// TODO no estoy seguro como va el server_name
	std::vector<int> listens = config.getListen();
	// for (size_t i = 0; i < listens.size(); i++)
	// {
	// 	sockaddr_in servAddr;
	// 	servAddr.sin_family = AF_INET;
	// 	servAddr.sin_port = htons(listens[i]);
	// 	// 0.0.0.0 permite todas las conexiones inet_addr() para convertir el string de la ip
	// 	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	// 	if (socketFd < 0)
	// 		throw CreateSocketException();

	// 	if (bind(socketFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	// 		throw BindingException();
	// 	listen(socketFd, 5);
	// 	_socketFd.insert(_socketFd.end(), socketFd);
	// 	_servAddr.insert(_servAddr.end(), servAddr);
	// }
}
// Server::Server(std::string ip, int port) : _ip(ip), _port(port)
// {
// 	_servAddr.sin_family = AF_INET;
// 	_servAddr.sin_port = htons(_port);
// 	// 0.0.0.0 permite todas las conexiones
// 	_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	// _servAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
// 	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (_socketFd < 0)
// 		throw CreateSocketException();

// 	if (bind(_socketFd, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) < 0)
// 		throw BindingException();
// 	listen(_socketFd, 5);
// }

Server::~Server()
{
}

std::vector<int> Server::getSocketFd() const
{
	return _socketFd;
}

std::vector<int> Server::getPorts() const
{
	return _config.getListen();
}

