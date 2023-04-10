#include "Request.hpp"
#include "Logs/Log.hpp"
#include "Utils/Strings.hpp"
#include "Utils/Colors.hpp"
#include "CGI.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <stdlib.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <functional>

Request::Request()
{
}

Request::Request(std::string req)
{
	// if (req.empty())
	// 	throw myException("Empty request", 0);
	std::cout << "REQ\n" << req << std::endl;
	tokenRequest(req);
}

/**
 * @brief MAPSPLIT
 * Nos envian un vector con un split por \n y un delimitador
 * Eliminamos la primera línea y nos quedamos con el resto hasta "Connection: close"
 *
 * @param auxVector
 * @param delimiter
 * @return std::map<std::string, std::string>
 */
std::map<std::string, std::string> mapSplit(std::vector<std::string> auxVector, std::string delimiter)
{
	std::map<std::string, std::string> auxMap;
	std::vector<std::string> newVector;
	std::vector<std::string>::iterator firstIter;
	std::vector<std::string>::iterator secondtIter;
	std::vector<std::string>::iterator itVector = auxVector.begin();

	itVector++;
	for (; itVector != auxVector.end() - 1; itVector++)
	{
		if ((*itVector).find(":") != std::string::npos)
		{
			newVector = Strings::split(*itVector, ":");
			firstIter = newVector.begin();
			secondtIter = newVector.begin();
			secondtIter++;
			if (newVector.size() > 2)
			{
				size_t size = 2;
				while (size != newVector.size())
				{
					*secondtIter = *secondtIter + delimiter + *(++secondtIter);
					size++;
				}
			}
			auxMap.insert(std::pair<std::string, std::string>(*firstIter, *secondtIter));
			if (*itVector == "Connection: close")
				return (auxMap);
		}
	}
	newVector = Strings::split(*itVector, ":");
	firstIter = newVector.begin();
	auxMap.insert(std::pair<std::string, std::string>(*firstIter, ""));
	return (auxMap);
}

/**
 * @brief Parseamos de la primera línea hasta el primer espacio
 *
 * @param lineVector
 */
void Request::setMethod(std::vector<std::string> lineVector)
{
	std::vector<std::string> newVector;
	std::vector<std::string>::iterator iter = lineVector.begin();

	while (iter != lineVector.end() && ((*iter).find("HTTP/1.1") == std::string::npos))
	{
		iter++;
	}
	if (iter == lineVector.end())
		throw myException("Incorrect method", 501);
	newVector = Strings::split(*iter, " /");
	iter = newVector.begin();
	_method = *iter;

	if (_method != "DELETE" && _method != "GET" && _method != "POST" && _method != "PUT" && _method != "PATCH" && _method != "HEAD")
		throw myException("Incorrect method", 501);
}

void Request::setHttp(std::vector<std::string> lineVector)
{
	std::vector<std::string> newVector;
	std::vector<std::string>::iterator iter;

	iter = lineVector.begin();
	newVector = Strings::split(*iter, " ");
	iter = newVector.begin();
	iter++;
	iter++;
	_http = *iter;
}

/**
 * @brief Parseamos de la primera línea a partir del primer espacio
 *
 * @param lineVector
 */
void Request::setPath(std::vector<std::string> lineVector)
{
	std::vector<std::string> newVector;
	std::vector<std::string>::iterator iter;

	iter = lineVector.begin();
	newVector = Strings::split(*iter, " ");
	if (newVector.empty())
		throw myException("Error in path", 0);
	iter = newVector.begin();
	iter++;
	if ((*iter).find("/") != std::string::npos)
		_path = Strings::trim(*iter, "/");
}

void Request::setHeader(std::vector<std::string> lineVector)
{
	_headers = mapSplit(lineVector, ":");

	for (std::map<std::string, std::string>::iterator iter = _headers.begin(); iter != _headers.end(); iter++)
	{
		if ((_headers.find("\n")) != _headers.end())
		{
			_headers.erase(iter, _headers.end());
			break;
		}
	}
}

void Request::setHostPort(std::vector<std::string> lineVector)
{
	std::vector<std::string> auxVector;
	std::vector<std::string>::iterator iter = lineVector.begin();

	for (std::vector<std::string>::iterator auxIter; iter < lineVector.end(); iter++)
	{
		if ((*iter).find("Host: ") != std::string::npos)
		{
			auxVector = Strings::split(*iter, " ");
			iter = auxVector.begin();
			iter++;
			auxVector = Strings::split(*iter, ":");
			auxIter = auxVector.begin();
			_host = *auxIter;
			auxIter++;
			_port = std::stoi(*auxIter);
			break;
		}
	}
}

/**
 * @brief If the method is not correct, we send and error
 * @return true
 * @return false
 */
int Request::errorsToken()
{
	if (_method != "DELETE" && _method != "GET" && _method != "POST" && _method != "PUT" && _method != "PATCH")
	{
		throw myException("Incorrect method", 501);
	}
	if (_http != "HTTP/1.1")
		throw myException("Incorrect protocol", 505);
		return (505);
	return (0);
}

/**
 * @brief Guardamos lo que hay despues de "boundary="
 * Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryIbYhOcGT9mbVEAXA
 *
 */
void Request::setBoundary(void)
{
	std::map<std::string, std::string>::iterator iterMap;

	try
	{
		std::string content = _headers.at("Content-Type");
		if (content.find("boundary") == std::string::npos)
			return;
		std::vector<std::string> boundary = Strings::split(content, "=");
		std::vector<std::string>::iterator iterV = boundary.begin();
		iterV++;
		// Le añade -- y al final en el form data y no esta en el boundary
		_boundary = "--";
		_boundary += *iterV;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}

/**
 * @brief Check si hay un Content-type
 *
 * @return true
 * @return false
 */
bool Request::isContentType(void)
{
	if ((_headers.find("Content-Type")) != _headers.end())
		return (true);
	return (false);
}

void Request::setPayload(std::vector<std::string> rawBoundary)
{
	std::vector<std::string>::iterator iterV = rawBoundary.begin();

	for (; iterV != rawBoundary.end(); iterV++)
	{
		if ((*iterV).empty())
			continue;
		if ((*iterV).find("name=") != std::string::npos)
		{
			// Comprobar si es un filename
			int i = (*iterV).find("\"");
			i++;
			std::string aux = (*iterV).substr(i, (*iterV).length() - i);
			int end = (aux).find("\"");
			// Extraemos el content-type
			std::string contentType = "Content-Type: ";
			int startType = (*iterV).find(contentType);
			startType += contentType.length();
			std::string auxType = (*iterV).substr(startType, 50);
			int endType = auxType.find("\n");
			contentType = auxType.substr(0, endType);
			// Extraemos el name
			std::string name = (*iterV).substr(i, end);
			int startFile = (*iterV).find("filename=");
			if (startFile > 0)
			{
				std::string filenameKey = "filename=\"";
				startFile += filenameKey.length();
				std::string aux = (*iterV).substr(startFile, (*iterV).length() - startFile);
				int end = (aux).find("\"");
				std::string content = (*iterV).substr(startFile, end);
				// TODO tenemos que ver en que path se crea
				std::ofstream file(content, std::ios::binary);
				std::string search = "filename=\"" + content + "\"";
				int start1 = _auxReq.find(search);
				// TODO comprobar que lo encuentra
				std::string sample = _auxReq.substr(start1, _auxReq.length() - start1);
				int start2 = sample.find("\r\n\r\n");
				start2 = start2 + 4;
				int end1 = sample.find(_boundary + "--");
				std::string fileRawContent = sample.substr(start2, end1 - start2);
				file << fileRawContent;
				file.close();
				// TODO poner el path correcto, en otra carpeta temporal;
				fileStruct fileData = {content, contentType, "./"};
				std::map<std::string, fileStruct>::iterator it = _files.end();
				_files.insert(it, std::pair<std::string, fileStruct>(name, fileData));
			}
			else
			{
				// Extraemos el content
				int n = (*iterV).find("\n\n");
				n++;
				std::string content = Strings::ltrim((*iterV).substr(n, (*iterV).length() - n - 1), "\n");

				std::map<std::string, std::string>::iterator it = _payload.end();
				_payload.insert(it, std::pair<std::string, std::string>(name, content));
			}
		}
	}
}

std::string setRawHeader(std::string str)
{
	// Limpia los caracteres de \e que vienen el el socket
	str.erase(std::remove(str.begin(), str.end(), 13), str.end());
	int i = str.find("\n\n");
	return (str.substr(0, i));
}

std::string setRawBody(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), 13), str.end());
	int i = str.find("\n\n");
	i++;
	return (str.substr(i, str.length() - i));
}

/**
 * @brief Creamos un token para method, path, header y body
 *
 */
int Request::tokenRequest(std::string req)
{
	std::string oldReq = Strings::ltrim(req, "\n");
	std::vector<std::string> clean = Strings::split(oldReq, "\n");

	bool init = false;
	req = "";
	for (size_t i = 0; i < clean.size(); i++)
	{
		if (clean[i].find("HTTP/1.1") != std::string::npos)
		{
			init = true;
		}
		if (init)
			req += clean[i] + "\n";
	}

	_auxReq = req;
	// Obtenemos el header
	std::string rawHeader = setRawHeader(req);
	// Obtenemos el body
	std::string rawBody = setRawBody(req);
	// if (rawHeader.empty())
	// 	throw myException("Empty request", 0);
	std::vector<std::string> lineVector = Strings::split(rawHeader, "\n");

	// if (lineVector.empty())
	// 	throw myException("Empty request", 0);

	setMethod(lineVector);
	setHttp(lineVector);
	setPath(lineVector);
	setHeader(lineVector);
	setHostPort(lineVector);
	if (!isContentType() || _boundary.empty())
		return 0;

	setBoundary();
	std::vector<std::string> rawBoundary = Strings::split(rawBody, _boundary);
	setPayload(rawBoundary);

	return (0);
}

std::string Request::getMethod(void) const
{
	return (_method);
}

std::string Request::getPath(void) const
{
	return (_path);
}

int Request::getPort() const
{
	return _port;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return (_headers);
}

std::map<std::string, std::string> Request::getPayload(void) const
{
	return (_payload);
}

std::map<std::string, fileStruct> Request::getFiles(void) const
{
	return (_files);
}

const char *Request::InvalidMethod::what() const throw()
{
	return "Invalid method";
}

const char *Request::InvalidProtocol::what() const throw()
{
	return "Invalid protocol";
}

std::ostream &operator<<(std::ostream &out, const Request &value)
{
	out << BLU "method: " RESET << value.getMethod() << std::endl;
	out << BLU "path: " RESET << value.getPath() << std::endl;
	std::map<std::string, std::string> _payload = value.getPayload();
	std::map<std::string, fileStruct> _files = value.getFiles();
	std::map<std::string, std::string>::iterator it = _payload.begin();
	out << BLU "payload: " RESET << std::endl;
	while (it != _payload.end())
	{
		std::string key = it->first;
		std::string value = it->second;
		out << BLU "" << key << " => " RESET << value << std::endl;
		it++;
	}
	std::map<std::string, fileStruct>::iterator it2 = _files.begin();
	while (it2 != _files.end())
	{
		std::string key = it2->first;
		fileStruct value = it2->second;
		out << key << " => {" << std::endl;
		out << BLU "\tname: " RESET << value.name << std::endl;
		out << BLU "\ttype: " RESET << value.type << std::endl;
		out << BLU "\ttmp_name: " RESET << value.tmp_name << std::endl;
		out << "}\n";
		it2++;
	}
	return (out);
}
