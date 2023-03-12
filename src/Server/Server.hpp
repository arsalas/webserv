#pragma once

#include <iostream>
#include <netinet/in.h>
 
class Server
{
private:
	int _config;
	std::string _ip;
	int _port;
	int _socketFd;
	sockaddr_in _servAddr;

public:
	Server();
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