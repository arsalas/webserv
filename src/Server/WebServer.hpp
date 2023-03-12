#pragma once

#include <iostream>
#include <vector>
#include <poll.h>
#include "Server.hpp"

#define RECV_BUFFER_SIZE 10024

class WebServer
{
public:
	static const int timeout = -1;

private:
	std::string _path;
	std::vector<Server> _servers;
	struct pollfd *_poll;

public:
	WebServer();
	~WebServer();

private:
	void constructPoll();
	void initPoll();
	void recivedPoll();
	void sendResponse(int fd, Server server);

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
};