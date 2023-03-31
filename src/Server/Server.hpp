#pragma once

#include <iostream>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <set>
#include "Config.hpp"

class Server
{
private:
	std::vector<sockaddr_in> _servAddr;
	std::vector<int> _socketFd;
	Config _config;


	// std::string _ip;
	// int _port;
	// int _socketFd;

public:
	int _id;
	std::vector<Config> _configServer;

public:
	Server();
	Server(Config);
	// Server(std::string, int);
	~Server();

	// GETTERS
	std::vector<int> getSocketFd() const;
	std::vector<int> getPorts() const;

private:
	// EXCEPTIONS
public:
};