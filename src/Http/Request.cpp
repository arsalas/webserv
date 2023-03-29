#include "Request.hpp"
#include "Logs/Log.hpp"
#include "Utils/Strings.hpp"
#include "Utils/Colors.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <stdlib.h>

Request::Request()
{
}

Request::Request(std::string req)
{

	// std::cout << req << std::endl;
	tokenRequest(req);
	// TODO
	// excepciones
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
			newVector = Strings::vectorSplit(*itVector, ":");
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
			// if ((*itVector).find("WebKitFormBoundary") != std::string::npos)
			// {
			//     std::cout << "HE SALIDO\n";
			//     return (auxMap);
			// }
		}
	}
	newVector = Strings::vectorSplit(*itVector, ":");
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
	std::vector<std::string>::iterator iter;

	iter = lineVector.begin();
	newVector = Strings::vectorSplit(*iter, " /");
	iter = newVector.begin();
	_method = *iter;
}

void Request::setHttp(std::vector<std::string> lineVector)
{
	std::vector<std::string> newVector;
	std::vector<std::string>::iterator iter;

	iter = lineVector.begin();
	newVector = Strings::vectorSplit(*iter, " ");
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
	newVector = Strings::vectorSplit(*iter, " ");
	iter = newVector.begin();
	iter++;
	_path = *iter;
}

void Request::setHeader(std::vector<std::string> lineVector)
{
	_header = mapSplit(lineVector, ":");

	for (std::map<std::string, std::string>::iterator iter = _header.begin(); iter != _header.end(); iter++)
	{
		if ((_header.find("\n")) != _header.end())
		{
			std::cout << "DENTRO DEL IF DE 2 nn\n";
			_header.erase(iter, _header.end());
			break ;
		}
	}
}

/**
 * @brief Parseamos el body
 * Hacemos match con "Connection: close"
 * Y a partir de ahí insertamos dentro del body
 *
 * @param lineVector
 */
void Request::setBody(std::vector<std::string> lineVector)
{
	for (std::vector<std::string>::iterator iter = lineVector.begin(); iter != lineVector.end(); iter++)
	{
		if (*iter == "Connection: close")
		{
			iter = iter + 2;
			while (iter != lineVector.end() - 1)
			{
				_body = _body + *iter;
				iter++;
			}
		}
	}
}

void Request::setHostPort(std::vector<std::string> lineVector)
{
	(void)lineVector;
	std::vector<std::string> auxVector;
	std::vector<std::string>::iterator iter = lineVector.begin();

	for (std::vector<std::string>::iterator auxIter; iter < lineVector.end(); iter++)
	{
		if ((*iter).find("Host: ") != std::string::npos)
		{
			auxVector = Strings::vectorSplit(*iter, " ");
			iter = auxVector.begin();
			iter++;
			auxVector = Strings::vectorSplit(*iter, ":");
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
 * // TODO Alberto, que mas??
 *
 * @return true
 * @return false
 */
int Request::errorsToken()
{
	if (_method != "DELETE" && _method != "GET" && _method != "POST" && _method != "PUT" && _method != "PATCH")
		return (501); // crear excepcion
	if (_http != "HTTP/1.1")
		return (505);
	return (0);
}

void Request::setFormData(std::vector<std::string> lineVector)
{
	std::vector<std::string>::iterator iter = lineVector.begin();
	std::vector<std::string>::iterator auxIter;
	std::vector<std::string> auxVector;

	for (std::vector<std::string>::iterator auxIter; iter < lineVector.end(); iter++)
	{
		if ((*iter).find("/?") != std::string::npos)
		{
			auxVector = Strings::vectorSplit(*iter, " ");
			auxIter = auxVector.begin();
			auxIter++;
			auxVector = Strings::vectorSplit(*auxIter, "&");
			auxIter = auxVector.begin();
			auxIter++;
		}
	}
}

/**
 * @brief Guardamos el nombre del archivo y la extensión cuando nos envían un documento
 *
 */
// void Request::setFilename(void)
// {
// 	std::vector<std::string>::iterator iter = _ContentDisposition.begin();
// 	std::vector<std::string> auxVector;
// 	std::vector<std::string>::iterator auxIter;

// 	for (; iter != _ContentDisposition.end(); iter++)
// 	{
// 		if ((*iter).find("filename") != std::string::npos)
// 		{
// 			std::cout << "YES I HAVE FIND IT!!!\n";
// 			auxVector = Strings::vectorSplit(*iter, ";");
// 			auxIter = auxVector.begin();
// 			for (; auxIter != auxVector.end(); auxIter++)
// 			{
// 				if ((*auxIter).find("filename") != std::string::npos)
// 				{
// 					auxVector = Strings::vectorSplit(*auxIter, "=");
// 					iter = auxVector.begin();
// 					iter++;
// 					auxVector = Strings::vectorSplit(*iter, ".");
// 					iter = auxVector.begin();
// 					_filename = Strings::deleteQuotes(*iter);
// 					iter++;
// 					std::cout << YEL << *iter << std::endl;
// 					_extension = Strings::deleteQuotes(*iter);
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	}
// }

/**
 * @brief Cuando envían un archivo, debemos crearlo
 *
 */
void Request::createFilename(void)
{
	if (!_filename.empty())
	{
		std::cout << "YES, ITS NOT EMPTY\n";
		// std::ofstream file(_filename + "." + _extension, std::ios::out);
		std::string name = "";
		name += (_filename + "." + _extension);
		std::cout << GRN "filename: ->" << _filename << "<-" RESET << std::endl;
		std::cout << GRN "extension: ->" << _extension << "<-" RESET << std::endl;
		std::ofstream file(name, std::ios_base::binary);
		std::cout << RED << _filename << "." << _extension << std::endl;
		std::cout << GRN "is open: " RESET << file.is_open() << std::endl;
		for (std::vector<std::string>::iterator iter = _fileContent.begin(); iter != _fileContent.end(); iter++)
		{
			// std::cout << "MY ITER IS: " << *iter << std::endl;
			file << *iter;
			file << "\n";

			// file2 << "console.log()\n";
		}
		file.close();
	}
}

// void Request::setFileContent(std::vector<std::string> lineVector)
// {
// 	std::vector<std::string>::iterator iter = lineVector.begin();
// 	std::vector<std::string>::iterator cdIter;

// 	for (; iter != lineVector.end() && iter < lineVector.end(); iter++)
// 	{
// 		if (iter != lineVector.end() && iter < lineVector.end())
// 		{
// 			if ((*iter).find("filename=") != std::string::npos)
// 			{
// 				iter++;
// 				iter++;
// 				while (iter != lineVector.end() && (*iter).find("Content-Disposition:") == std::string::npos && (*iter).find("WebKitFormBoundary:") == std::string::npos)
// 				{
// 					cdIter = _fileContent.end();
// 					_fileContent.insert(cdIter, *iter);
// 					iter++;
// 				}
// 			}
// 		}
// 	}
// }

void	Request::setFileName(std::string str)
{
	std::vector<std::string> filename = Strings::vectorSplit(str, "filename");
	std::vector<std::string>::iterator iter = filename.begin();
	iter++;
	_filename = (*iter).substr(1, (*iter).length() -2);
}

/**
 * @brief Cuando envían un archivo, debemos guardar su contenido
 *
 * @param lineVector
 */
void Request::setFile(std::vector<std::string> lineVector)
{
	std::vector<std::string>::iterator iter = lineVector.begin();
	std::vector<std::string>::iterator cdIter;

	for (; iter != lineVector.end() && (*iter).find(_boundary) != std::string::npos; iter++)
	{
	}
	for (; iter != lineVector.end(); iter++)
	{
		if ((*iter).find("filename=") != std::string::npos)
		{
			setFileName(*iter);
			// TODO guardar contenido
		}
	}
	// if ((*iter).find("Content-Disposition:") != std::string::npos)
	// {
	// 	cdIter = _ContentDisposition.end();
	// 	_ContentDisposition.insert(cdIter, *iter);
	// }
}

/**
 * @brief Guardamos lo que hay despues de "boundary="
 * Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryIbYhOcGT9mbVEAXA
 * 
 */
void Request::setBoundary( void )
{
	std::map<std::string, std::string>::iterator iterMap;

	iterMap = _header.find("Content-Type");
	std::cout << "iterMap is: " << _header.find("Content-Type")->first << std::endl;
	std::cout << "iterMap is: " << _header.find("Content-Type")->second << std::endl;
	std::vector<std::string> boundary = Strings::vectorSplit(iterMap->second, "=");
	std::vector<std::string>::iterator iterV = boundary.begin();
	iterV++;
	_boundary = *iterV;
	std::cout << "BOUNDARY IS: " << *iterV << std::endl;
}

/**
 * @brief Check si hay un Content-type
 * 
 * @return true 
 * @return false 
 */
bool Request::isContentType( void )
{
	if ((_header.find("Content-Type")) != _header.end())
		return (true);
	return (false);
}

void	Request::setMapFiles(std::vector<std::string> lineVector)
{
	std::vector<std::string>::iterator iterV = lineVector.begin();
	std::vector<std::string>::iterator iterName;
	std::vector<std::string> name;

	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
			break ;
	}
	iterV++;
	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
		{
			break ;
		}
	}
	iterV++;
	std::cout << "ITERV IS: " << *iterV << std::endl;
	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
		{
			iterV++;
			std::cout << "INSIDE THE IF: " << *iterV << std::endl;
			name = Strings::vectorSplit(*iterV, "name=");
			if (!name.empty())
			{
				iterName = name.begin();
				iterName++;
				std::string name = (*iterName).substr(0, (*iterName).find(";"));
				std::cout << "NAME IS " << name << std::endl;

				std::map<std::string, std::string>::iterator it = _mapFiles.end();
				_mapFiles.insert(it, std::pair<std::string, std::string>(_filename, name));
				// _mapFiles.insert (std::pair<std::string, std::string>(_filename, name));
				// _mapFiles[_filename] = name;
			}
			break ;
		}
	}
}

void	Request::setPayload(std::vector<std::string> lineVector)
{
	std::vector<std::string>::iterator iterV = lineVector.begin();
	std::vector<std::string>::iterator iterName;
	std::vector<std::string> name;
	std::string payloadKey;

	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
			break ;
	}
	iterV++;
	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
		{
			iterV++;
			name = Strings::vectorSplit(*iterV, "name=");
			if (!name.empty())
			{
				iterName = name.begin();
				iterName++;
				payloadKey = *iterName;
			}
			break ;
		}
	}
	std::cout << "PAYLOAD IS " << payloadKey << std::endl;
	iterV++;
	for (; iterV != lineVector.end(); iterV++)
	{
		if ((*iterV).find(_boundary) != std::string::npos)
		{
			iterV++;
			if (!name.empty())
			{
				name = Strings::vectorSplit(*iterV, "name=");
				iterName = name.begin();
				iterName++;
				std::string payloadContent = (*iterName).substr(0, (*iterName).find(";"));
				std::cout << "ITERNAME IS " << payloadContent << std::endl;
				std::map<std::string, std::string>::iterator it = _mapPayload.end();
				_mapPayload.insert(it, std::pair<std::string, std::string>(payloadKey, payloadContent));
				// _mapPayload.insert (std::pair<std::string, std::string>(payloadKey,payloadContent));
				// _mapPayload[payloadKey] = payloadContent;
			}
			break ;
		}
	}
}

std::string	setRawHeader(std::string str)
{
	std::string newStr;
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == '\n' && str[i + 1] == '\n')
		{
			i += 2;
			std::cout << "I HAVE FOUND IT. I WILL RETURN: " << str[i] << std::endl;
			newStr.substr(i, newStr.length());
			return (newStr);
		}
	}
	return (str);
}

std::string	setBody(std::string str)
{
	std::string newStr;
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == '\n' && str[i + 1] == '\n')
		{
			i += 2;
			std::cout << "I HAVE FOUND IT. I WILL RETURN: " << str[i] << str[i+1] << str[i+3] << std::endl;
			newStr.substr(0, i);
			return (newStr);
		}
	}
	return (str);
}

/**
 * @brief Creamos un token para method, path, header y body
 *
 */
int Request::tokenRequest(std::string req)
{
	std::vector<std::string> lineVector;

	std::cout << "\n ALL MY REQUEST IS\n" << req << std::endl;

	std::string rawHeader = setRawHeader(req);
	// std::string body = setBody(req);
	std::cout << "I HAVE RAWHEADER\n";
	lineVector = Strings::vectorSplit(req, "\n");
	setMethod(lineVector);
	std::cout << "METHOD: " << _method << std::endl;
	setHttp(lineVector);
	std::cout << "HTTP: " << _http << std::endl;
	setPath(lineVector);
	std::cout << "PATH: " << _path << std::endl;
	setHeader(lineVector);
	for (std::map<std::string, std::string>::iterator iterMap  = _header.begin(); iterMap != _header.end(); iterMap++)
	{
	    std::cout << "HEADER: " << iterMap->first << " & " << iterMap->second << std::endl;
	}
	setHostPort(lineVector);
	std::cout << "PORT: " << _port << std::endl;
	// setFormData(lineVector);
	// setBody(lineVector); // TODO Alberto, al final lo utilizamos???
	if (isContentType())
	{
		setBoundary();
		setFile(lineVector);
		setPayload(lineVector);
		setMapFiles(lineVector);
		std::cout << "Inside content disposition\n";
	}
	std::map<std::string, std::string>::iterator itM;
	for (itM = _mapPayload.begin(); itM != _mapPayload.end(); itM++)
	{
		std::cout << "MY PAYLOAD IS: " << itM->first << " | " << itM->second << std::endl;
	}
	for (itM = _mapFiles.begin(); itM != _mapFiles.end(); itM++)
	{
		std::cout << "MY mapFiles IS: " << itM->first << " | " << itM->second << std::endl;
	}
	// setFileContent(lineVector);
	// createFilename();

	return (errorsToken());
	return (0);
}

std::string Request::getMethod(void) const
{
	return (_method);
}

std::string Request::getHttp(void) const
{
	return (_http);
}

std::string Request::getPath(void) const
{
	return (_path);
}

std::map<std::string, std::string> Request::getHeader(void) const
{
	return (_header);
}

std::string Request::getBody(void) const
{
	return (_body);
}

int Request::getPort(void) const
{
	return (_port);
}

std::string Request::getHost(void) const
{
	return (_host);
}

// int main ()
// {
//     Request re;
//     re.tokenRequest();
//     return (0);
// }
