#pragma once

#include <iostream>
#include <map>
#include "StatusCode.hpp"

class Response
{

private:
	int _fd;
	int _code;
	StatusCode _statusCode;
	std::string _body;
	std::map<std::string, std::string> _headers;

private:
	Response();
	std::string getHeaders();

public:
	Response(int code);
	~Response();

	size_t end();
	size_t render(std::string);
	size_t sendFile(std::string filename);
	size_t attachment(std::string filename);

	Response status(int code);
	Response append(std::string, std::string);
	Response type(std::string contentType);

};