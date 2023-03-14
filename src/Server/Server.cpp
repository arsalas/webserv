#include "Server.hpp"

Server::Server() {}
Server::Server(Config config) : _config(config)
{
	std::vector<int> listens = config.getListen();
}

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

