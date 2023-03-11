#include "Request.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <vector>


std::vector<std::string>    vectorSplit(std::string file, std::string delimiter)
{
    std::vector<std::string> auxVector;
    std::vector<std::string>::iterator  it = auxVector.begin();
    std::string aux;
    size_t pos = 0;

    while ((pos = file.find(delimiter)) != std::string::npos)
    {
        aux = file.substr(0, pos);
        it = auxVector.end();
        auxVector.insert(it, aux);
        file.erase(0, pos + delimiter.length());
    }
    aux = file.substr(0, pos);
    it = auxVector.end();
    auxVector.insert(it, aux);
    return (auxVector);
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
    for (; itVector != auxVector.end() -1; itVector++)
    { 
        newVector = vectorSplit(*itVector, ":"); 
        firstIter = newVector.begin();
        secondtIter = newVector.begin();
        secondtIter++;
        size_t size = 2;
        if (newVector.size() > 2)
        {
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
    newVector = vectorSplit(*itVector, ":"); 
    firstIter = newVector.begin();
    auxMap.insert(std::pair<std::string, std::string>(*firstIter, ""));
    return (auxMap);
}

/**
 * @brief Parseamos de la primera línea hasta el primer espacio
 * 
 * @param lineVector 
 */
void    Request::setMethod(std::vector<std::string> lineVector)
{
    std::vector<std::string> newVector;
    std::vector<std::string>::iterator iter;

    iter = lineVector.begin();
    newVector = vectorSplit(*iter, " /");
    iter = newVector.begin();
    _method = *iter;
}

/**
 * @brief Parseamos de la primera línea a partir del primer espacio
 * 
 * @param lineVector 
 */
void    Request::setPath(std::vector<std::string> lineVector)
{
    std::vector<std::string> newVector;
    std::vector<std::string>::iterator iter;

    iter = lineVector.begin();
    newVector = vectorSplit(*iter, " ");
    iter = newVector.begin();
    iter++;
    _path = *iter;
}

void    Request::setHeader(std::vector<std::string> lineVector)
{
    _header = mapSplit(lineVector, ":");
}

/**
 * @brief Parseamos el body
 * Hacemos match con "Connection: close"
 * Y a partir de ahí insertamos dentro del body
 * 
 * @param lineVector 
 */
void    Request::setBody(std::vector<std::string> lineVector)
{
    for (std::vector<std::string>::iterator iter = lineVector.begin(); iter != lineVector.end(); iter++)
    {
        if (*iter == "Connection: close")
        {
            iter = iter + 2;
            while (iter != lineVector.end() -1)
            {
                _body = _body + *iter;
                iter++;
            }
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
bool    Request::errorsToken()
{
    if (_method != "DELETE" && _method != "GET" && _method != "POST")
        return (1);
    // if (_path[0] != '\\')
    //     return (1);
    return (0);
}

/**
 * @brief Creamos un token para method, path, header y body
 * Printamos el log
 * 
 */
void Request::tokenRequest(void)
{
    std::string file = "DELETE /gfdgdf HTTP/1.1\nuser-agent: Thunder Client (https://www.thunderclient.com)\naccept: */*\nauthorization: 13245687\ncontent-type: application/json\ncontent-length: 23\naccept-encoding: gzip, deflate, br\nHost: 127.0.0.1:3001\nConnection: close\n{\n\"name\": \"Hola\"\n}; ";
    // std::string file = "GET /favicon.ico HTTP/1.1\nHost: 127.0.0.1:8080\nConnection: keep-alive\nsec-ch-ua: \"Chromium\";v=\"110\", \"Not A(Brand\";v=\"24\", \"Google Chrome\";v=\"110\"\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\nsec-ch-ua-platform: \"macOS\"\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://127.0.0.1:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,es;q=0.8";
    std::vector<std::string> lineVector;

    lineVector = vectorSplit(file, "\n");
    setMethod(lineVector);
    setPath(lineVector);
    setHeader(lineVector);
    setBody(lineVector);
    if (errorsToken()) // TODO no funciona el error, da segfault
        throw std::runtime_error(std::string("Error in the request file\n"));
    else
        Log::printLog(_method);

    std::cout << "METHOD: " << _method << std::endl;
    std::cout << "PATH: " << _path << std::endl;
    for (std::map<std::string, std::string>::iterator iterMap  = _header.begin(); iterMap != _header.end(); iterMap++)
    {
        std::cout << "MAP: " << iterMap->first << " & " << iterMap->second << std::endl;
    }
    std::cout << "BODY: " << _body << std::endl;
}

std::string Request::getMethod(void)
{
    return (_method);
}

std::string Request::getPath(void)
{
    return (_path);
}

std::map<std::string, std::string> Request::getHeader(void)
{
    return (_header);
}

std::string Request::getBody(void)
{
    return (_body);
}

int main ()
{
    Request re;
    re.tokenRequest();
    return (0);
}
