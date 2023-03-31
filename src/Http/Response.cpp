#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "Response.hpp"
#include "Utils/Strings.hpp"
#include "Utils/File.hpp"
#include "Utils/Dirs.hpp"
#include "Pages/Autoindex.hpp"

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
	std::cout << "TYPE: " << _mimeTypes[File::getExtension(filename)] << std::endl;
	type(_mimeTypes[File::getExtension(filename)]);
	try
	{
		File file(filename);
		_body = file.toStr();
		return end();
	}
	catch (const std::exception &e)
	{
		// TODO esto no va aqui, hay que buscar el path en el server
		(void)e;
		status(404);
		return sendFile("src/Templates/InternalServerError.html");
	}
}
size_t Response::attachment(std::string filename)
{
	std::cout << "TYPE: " << _mimeTypes[File::getExtension(filename)] << std::endl;
	type(_mimeTypes[File::getExtension(filename)]);
	File file(filename);
	// Con el content-disposition se envian archivos
	std::string disposition = "attachment; filename=\"" + Dirs::getFilenameFromPath(filename) + "\"";
	std::cout << "disposition: " << disposition << std::endl;

	append("Content-Disposition", disposition);
	_body = file.toStr();
	status(200);
	return end();
}

size_t Response::redirect(std::string url)
{
	status(301);
	_headers["Location"] = url;
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

void Response::notAllowed()
{
	status(405);
	sendFile("src/Templates/NotAllowed.html");
}

void Response::notFound()
{
	status(404);
	sendFile("src/Templates/NotFound.html");
}

void Response::notFound(std::string path)
{
	status(404);
	sendFile(path);
}

void Response::limitExced()
{
	status(413);
	sendFile("src/Templates/PayloadTooLarge.html");
}

void Response::limitExced(std::string path)
{
	status(413);
	sendFile(path);
}

void Response::autoindex(std::string path, std::string root)
{
	status(200);
	std::cout << "Path: " << path << std::endl;
	Autoindex autoindex(path, root);
	render(autoindex.toStr());
}
