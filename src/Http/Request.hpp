#pragma once 

#include <iostream>
#include <map>
#include <vector>
#include <fstream>

#include "Utils/Structs.hpp"

class Request
{
private:
	// _headers;
	// _payload;
	// _files;
	// _path;
	// _method;

	std::string _auxReq;
	std::string _method;
	std::string _http;
	std::string _path;
	std::string _host;
	int _port;
	std::map<std::string, std::string> _headers;
	std::string _boundary;
	std::map<std::string, std::string> _payload;
	std::map<std::string, fileStruct> _files;

private:
	Request();
	int tokenRequest(std::string req);
	int errorsToken();

public:
	Request(std::string req);

	/*      SETTERS     */
	void setMethod(std::vector<std::string> lineVector);
	void setHttp(std::vector<std::string> lineVector);
	void setPath(std::vector<std::string> lineVector);
	void setHeader(std::vector<std::string> lineVector);
	void setHostPort(std::vector<std::string> lineVector);

	bool isContentType(void);
	void setBoundary(void);

	void setPayload(std::vector<std::string> lineVector);

	/*      GETTERS     */
	std::string getMethod() const;
	std::string getPath() const;
	int getPort() const;
	std::map<std::string, std::string> getHeaders() const;
	std::map<std::string, std::string> getPayload() const;
	std::map<std::string, fileStruct> getFiles() const;

public:
	class InvalidMethod : public std::exception
	{
		virtual const char *what() const throw();
	};

public:
	class InvalidProtocol : public std::exception
	{
		virtual const char *what() const throw();
	};
};

std::ostream &operator<<(std::ostream &out, const Request &value);
