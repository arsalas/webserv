#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "Response.hpp"
#include "Utils/Strings.hpp"
#include "Utils/File.hpp"

Response::Response()
{
}

Response::Response(int fd) : _fd(fd), _body("")
{
	type("text/html");
}

Response::~Response()
{
}

std::string Response::getHeaders()
{
	std::map<std::string, std::string>::iterator it;
	std::ostringstream ss;

	append("Content-Length", Strings::intToString(_body.size()));
	ss << "HTTP/1.1 " << _code << " " << _statusCode[_code] << " " << std::endl;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		ss << it->first << ": " << it->second << std::endl;
	}
	ss << "\n";
	return ss.str();
}

size_t Response::end()
{
	std::string resp = getHeaders();
	resp += _body;
	return send(_fd, resp.c_str(), resp.size(), 0);
}
size_t Response::render(std::string body)
{
	_body = body;
	return end();
}
size_t Response::sendFile(std::string filename)
{
	try
	{
		File file(filename);
		_body = file.toStr();
		return end();
	}
	catch (const std::exception &e)
	{
		(void)e;
		status(404);
		return sendFile("www/404.html");
	}

}
size_t Response::attachment(std::string filename)
{
	File file(filename);
	_body = file.toStr();
	return end();
}

Response Response::status(int code)
{
	_code = code;
	return *this;
}
Response Response::append(std::string key, std::string value)
{
	_headers[key] = value;
	return *this;
}
Response Response::type(std::string contentType)
{
	_headers["Content-Type"] = contentType + "; charset=UTF-8";
	return *this;
}
