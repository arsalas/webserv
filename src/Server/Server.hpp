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
	Config _config;
	std::vector<sockaddr_in> _servAddr;
	std::vector<int> _socketFd;

	// sockaddr_in _servAddr;
	// std::string _ip;
	// int _port;
	// int _socketFd;

public:
	Server();
	Server(Config);
	Server(std::string, int);
	~Server();

	// GETTERS
	int getSocketFd() const;
	std::vector<int> getPorts() const;

private:
	// EXCEPTIONS
public:
	class CreateSocketException : public std::exception
	{
		virtual const char *what() const throw();
	};
	class BindingException : public std::exception
	{
		virtual const char *what() const throw();
	};
};