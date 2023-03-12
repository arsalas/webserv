#pragma once

#include <iostream>
#include <netinet/in.h>
#include<vector>
#include<map>
#include "Config.hpp"
 
class Server
{
private:
	Config _config;
	sockaddr_in _servAddr;
	std::string _ip;
	int _port;
	int _socketFd;

public:
	Server();
	Server(Config);
	Server(std::string, int);
	~Server();

	// GETTERS
	int getSocketFd() const;

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