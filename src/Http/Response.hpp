#pragma once

#include <iostream>
#include <map>
#include "StatusCode.hpp"
#include "Utils/MimeTypes.hpp"

class Response
{
public:
private:
	int _fd;
	int _code;
	StatusCode _statusCode;
	MimeTypes _mimeTypes;
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
	size_t redirect(std::string url);

	Response status(int code);
	Response append(std::string, std::string);
	Response type(std::string contentType);

	void notAllowed();
	void notFound();
	void notFound(std::string path);
	void limitExced();
	void limitExced(std::string path);
	void sendError(std::string path);
	void autoindex(std::string path, std::string root);


};