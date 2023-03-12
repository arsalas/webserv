#include <arpa/inet.h>
#include "Server.hpp"

Server::Server() {}
Server::Server(Config config) : _config(config)
{
	// TODO no estoy seguro que el ip sea correcto con el server_name
	std::vector<int> listens = config.getListen();
	for (size_t i = 0; i < listens.size(); i++)
	{
		_servAddr.sin_family = AF_INET;
		_servAddr.sin_port = htons(_port);
		// 0.0.0.0 permite todas las conexiones inet_addr() para convertir el string de la ip
		_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (_socketFd < 0)
			throw CreateSocketException();

		if (bind(_socketFd, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) < 0)
			throw BindingException();
		listen(_socketFd, 5);
	}
}
Server::Server(std::string ip, int port) : _ip(ip), _port(port)
{
	_servAddr.sin_family = AF_INET;
	_servAddr.sin_port = htons(_port);
	// 0.0.0.0 permite todas las conexiones
	_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// _servAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0)
		throw CreateSocketException();

	if (bind(_socketFd, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) < 0)
		throw BindingException();
	listen(_socketFd, 5);
}

Server::~Server()
{
}

int Server::getSocketFd() const
{
	return _socketFd;
}

// Exceptions
const char *Server::CreateSocketException::what() const throw()
{
	return "Cannot create socket";
}
const char *Server::BindingException::what() const throw()
{
	return "ERROR on binding";
}