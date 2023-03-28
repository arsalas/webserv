#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <poll.h>
#include "Server.hpp"

#define RECV_BUFFER_SIZE 100
#define MAX_CLIENTS 5

class WebServer
{
public:
	static const int timeout = -1;

private:
	std::string _path;
	std::vector<Server> _servers;
	std::vector<struct pollfd> _poll;
	// std::vector<std::string> _fdContent = 5;
	std::string _fdContent[MAX_CLIENTS];

public:
	WebServer();
	~WebServer();

private:
	std::set<int> getPorts();
	void startSockets();
	void initPoll();
	void addPoll(int fd);
	void recivedPoll();
	void sendResponse(int fd);

	// EXCEPTIONS
public:
	class AcceptSocketException : public std::exception
	{
		virtual const char *what() const throw();
	};

	class RecivedSocketException : public std::exception
	{
		virtual const char *what() const throw();
	};

	class SendSocketException : public std::exception
	{
		virtual const char *what() const throw();
	};

	class CreateSocketException : public std::exception
	{
		virtual const char *what() const throw();
	};
	class BindingException : public std::exception
	{
		virtual const char *what() const throw();
	};
};