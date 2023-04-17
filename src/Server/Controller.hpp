#pragma once

#include <iostream>
#include <vector>

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Server/Server.hpp"

class Controller
{

private:
	Request _req;
	Response _res;
	Server _server;
	Controller();

public:
	Controller(std::vector<Server> servers, Request req, Response res);
	~Controller();

private:
	int findServer(std::vector<Server> servers);


	class ServerException : public std::exception
	{
		virtual const char *what() const throw();
	};
};
