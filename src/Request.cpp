#include "Request.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <stdlib.h>

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

bool    isHyphenDigit(std::string str)
{
    int c = 0;
    while (str[c] == '-' || isdigit(str[c]))
        c++;
    if (!str.empty() && str[c] == '\0')
    {
        return (1);
    }
    return (0);
}

bool    isContentDisposition(std::string str)
{
    if (str.find("Content-Disposition: form-data") != std::string::npos)
        return (1);
    return (0);
}

void    Request::setFormData(std::vector<std::string>::iterator itVector, std::vector<std::string> auxVector, std::vector<std::string>::iterator endVector)
{
    if (isHyphenDigit(*itVector))
    {
        std::vector<std::string> newVector;
        std::vector<std::string>::iterator firstIter;
        std::vector<std::string>::iterator secondIter;
        for (; itVector < endVector; itVector++)
        {
            if (itVector < endVector && !(*itVector).empty() && !isHyphenDigit(*itVector))
            {
                newVector = vectorSplit(*itVector, "Content-Disposition: form-data; name=");
                firstIter = (++newVector.begin()++);
                secondIter++;
                if (!(*firstIter).empty() )
                {
                    std::cout << "My first vector is: " << *(newVector.begin()) << std::endl;
                    std::cout << "My second vector is: " << *(++newVector.begin()) << std::endl;
                    firstIter = newVector.begin();
                    secondIter = newVector.begin();
                    
                    secondIter++;
                    _formData.insert(std::pair<std::string, std::string>(*firstIter, *secondIter));
                }
            }
        }
    }
}

// void    Request::setFormData(std::vector<std::string>::iterator itVector, std::vector<std::string> auxVector, std::vector<std::string>::iterator endVector)
// {
//     if (isContentDisposition(*itVector))
//     {
//         std::vector<std::string> newVector;
//         std::vector<std::string>::iterator firstIter;
//         std::vector<std::string>::iterator secondtIter;
//         for (; itVector < endVector; itVector++)
//         {
//             if (itVector < endVector && !(*itVector).empty() && !isContentDisposition(*itVector))
//             {
//                 newVector = vectorSplit(*itVector, ":");
//                 firstIter = (++newVector.begin());
//                 if (!(*firstIter).empty() )
//                 {
//                     std::cout << "My first vector is: " << *(newVector.begin()) << std::endl;
//                     std::cout << "My second vector is: " << *(++newVector.begin()) << std::endl;
//                     firstIter = newVector.begin();
//                     secondtIter = newVector.begin();
//                     secondtIter++;
//                     _formData.insert(std::pair<std::string, std::string>(*firstIter, *secondtIter));
//                 }
//             }
//         }
//     }
// }

/**
 * @brief MAPSPLIT
 * Nos envian un vector con un split por \n y un delimitador
 * Eliminamos la primera línea y nos quedamos con el resto hasta "Connection: close"
 * 
 * @param auxVector 
 * @param delimiter 
 * @return std::map<std::string, std::string> 
 */
std::map<std::string, std::string> Request::mapSplit(std::vector<std::string> auxVector, std::string delimiter)
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
        if (isHyphenDigit(*itVector))
        {
            setFormData(itVector, auxVector, auxVector.end() -1);
            return (auxMap);
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

void    Request::setHttp(std::vector<std::string> lineVector)
{
        std::vector<std::string> newVector;
    std::vector<std::string>::iterator iter;

    iter = lineVector.begin();
    newVector = vectorSplit(*iter, " ");
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

void    Request::setHostPort(std::vector<std::string> lineVector)
{
    (void) lineVector;
    std::vector<std::string> auxVector;
    std::vector<std::string>::iterator iter = lineVector.begin();
    std::vector<std::string>::iterator auxIter;
    for (; iter < lineVector.end(); iter++)
    {
        if ((*iter).find("Host: ") != std::string::npos)
        {
            auxVector = vectorSplit(*iter, " ");
            iter = auxVector.begin();
            auxVector = vectorSplit(*iter, ":");
            auxIter = auxVector.begin();
            _host = *auxIter;
            auxIter++;
            _port = *auxIter;
            // _port = std::stoi(*auxIter);
            break ;
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
    if (_method != "DELETE" && _method != "GET" && _method != "POST" && _method != "PUT") 
        return (501);
    if (_http != "HTTP/1.1")
        return (505);
    return (0);
}

/**
 * @brief Creamos un token para method, path, header y body
 * Printamos el log
 * 
 */
int Request::tokenRequest(void)
{
    std::string file = "DELETE /gfdgdf HTTP/1.1\nuser-agent: Thunder Client (https://www.thunderclient.com)\naccept: */*\nauthorization: 13245687\ncontent-type: application/json\ncontent-length: 23\naccept-encoding: gzip, deflate, br\nHost: 127.0.0.1:3001\nConnection: close\n{\n\"name\": \"Hola\"\n}; ";
    // std::string file = "GET /favicon.ico HTTP/1.1\nHost: 127.0.0.1:8080\nConnection: keep-alive\nsec-ch-ua: \"Chromium\";v=\"110\", \"Not A(Brand\";v=\"24\", \"Google Chrome\";v=\"110\"\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\nsec-ch-ua-platform: \"macOS\"\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://127.0.0.1:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,es;q=0.8";
    // std::string file = "POST / HTTP/1.1\nUser-Agent: PostmanRuntime/7.31.1\nAccept: */*\nPostman-Token: a440ce60-e830-46ec-924e-ca8d079f302d\nHost: 127.0.0.1:3002\nAccept-Encoding: gzip, deflate, br\nConnection: keep-alive\nContent-Type: multipart/form-data; boundary=--------------------------321951531579093893025774\nContent-Length: 145640\n\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"name\"\n\nhola\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"pass\"\n\nmundo\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"file\"; filename=\"code.png\"\nContent-Type: image/png";
    std::vector<std::string> lineVector;

    lineVector = vectorSplit(file, "\n");
    setMethod(lineVector);
    setHttp(lineVector);
    setPath(lineVector);
    setHeader(lineVector);
    setBody(lineVector);
    setHostPort(lineVector);

    std::cout << "METHOD: " << _method << std::endl;
    std::cout << "HTTP: " << _http << std::endl;
    std::cout << "PATH: " << _path << std::endl;
    for (std::map<std::string, std::string>::iterator iterMap  = _header.begin(); iterMap != _header.end(); iterMap++)
    {
        std::cout << "MAP: " << iterMap->first << " & " << iterMap->second << std::endl;
    }
    for (std::map<std::string, std::string>::iterator iter  = _formData.begin(); iter != _formData.end(); iter++)
    {
        std::cout << "FORMDATA: " << iter->first << " & " << iter->second << std::endl;
    }
    std::cout << "BODY: " << _body << std::endl;
    return (errorsToken());
}

std::string Request::getMethod(void)
{
    return (_method);
}

std::string Request::getHttp(void)
{
    return (_http);
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

std::string Request::getPort(void)
{
    return (_port);
}

std::string Request::getHost(void)
{
    return (_host);
}

int main ()
{
    Request re;
    re.tokenRequest();
    return (0);
}
