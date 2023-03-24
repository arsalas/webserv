#include "Request.hpp"
#include "Logs/Log.hpp"
#include "Utils/Strings.hpp"
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
    
    std::cout << req << std::endl;
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
    for (; itVector != auxVector.end() -1; itVector++)
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

void    Request::setHttp(std::vector<std::string> lineVector)
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
void    Request::setPath(std::vector<std::string> lineVector)
{
    std::vector<std::string> newVector;
    std::vector<std::string>::iterator iter;

    iter = lineVector.begin();
    newVector = Strings::vectorSplit(*iter, " ");
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
    if (_method != "DELETE" && _method != "GET" && _method != "POST" && _method != "PUT" && _method != "PATCH") 
        return (501); // crear excepcion
    if (_http != "HTTP/1.1")
        return (505);
    return (0);
}

void    Request::setFormData(std::vector<std::string> lineVector)
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
            std::cout << "AV: " << *auxIter << std::endl;
            auxIter++;
            std::cout << "AV: " << *auxIter << std::endl;
        }
    }
}

void    Request::setContentDisposition(std::vector<std::string> lineVector)
{
    std::vector<std::string>::iterator iter = lineVector.begin();

    for(; iter != lineVector.end(); iter++)
    {
        if ((*iter).find("WebKitFormBoundary") != std::string::npos)
        {
            *iter++;
            std::vector<std::string>::iterator cdIter = _ContentDisposition.end();
            for (; iter != lineVector.end(); iter++)
            {
                cdIter = _ContentDisposition.end();
                std::string str = *iter;
                _ContentDisposition.insert(cdIter, str);
                cdIter++;
            }
            std::cout << "B\n";
            break ;
        }
    }
}

/**
 * @brief Creamos un token para method, path, header y body
 * Printamos el log
 * 
 */
int Request::tokenRequest(std::string req)
{
    std::cout << "ENTRO DENTRO DE TOKEN REQUEST" << std::endl;
    // std::string req = "DELETE /gfdgdf HTTP/1.1\nuser-agent: Thunder Client (https://www.thunderclient.com)\naccept: */*\nauthorization: 13245687\ncontent-type: application/json\ncontent-length: 23\naccept-encoding: gzip, deflate, br\nHost: 127.0.0.1:3001\nConnection: close\n{\n\"name\": \"Hola\"\n}; ";
    // std::string file = "GET /favicon.ico HTTP/1.1\nHost: 127.0.0.1:8080\nConnection: keep-alive\nsec-ch-ua: \"Chromium\";v=\"110\", \"Not A(Brand\";v=\"24\", \"Google Chrome\";v=\"110\"\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\nsec-ch-ua-platform: \"macOS\"\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://127.0.0.1:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,es;q=0.8";
    // std::string file = "POST / HTTP/1.1\nUser-Agent: PostmanRuntime/7.31.1\nAccept: */*\nPostman-Token: a440ce60-e830-46ec-924e-ca8d079f302d\nHost: 127.0.0.1:3002\nAccept-Encoding: gzip, deflate, br\nConnection: keep-alive\nContent-Type: multipart/form-data; boundary=--------------------------321951531579093893025774\nContent-Length: 145640\n\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"name\"\n\nhola\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"pass\"\n\nmundo\n----------------------------321951531579093893025774\nContent-Disposition: form-data; name=\"file\"; filename=\"code.png\"\nContent-Type: image/png";
    // std::string req = "GET /?name=hjkhjk&name2=hjkhjk HTTP/1.1\nHost: 127.0.0.1:3001\nConnection: keep-alive\nsec-ch-ua: \"Google Chrome\";v=\"111\", \"Not(A:Brand\";v=\"8\", \"Chromium\";v=\"111\"\nsec-ch-ua-mobile: ?0\nsec-ch-ua-platform: \"macOS\"\nUpgrade-Insecure-Requests: 1\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: navigate\nSec-Fetch-User: ?1\nSec-Fetch-Dest: document\nReferer: http://127.0.0.1:3001/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,es;q=0.8,ru;q=0.7";
    std::vector<std::string> lineVector;
    // req = "GET / HTTP/1.1\nHost: localhost:8082\nConnection: keep-alive\nsec-ch-ua: \"Google Chrome\";v=\"111\", \"Not(A:Brand\";v=\"8\", \"Chromium\";v=\"111\"\nsec-ch-ua-mobile: ?0\nsec-ch-ua-platform: \"macOS\"\nUpgrade-Insecure-Requests: 1\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\nSec-Fetch-Site: none\nSec-Fetch-Mode: navigate\nSec-Fetch-User: ?1\nSec-Fetch-Dest: document\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9";

    lineVector = Strings::vectorSplit(req, "\n");
    setMethod(lineVector);
    setFormData(lineVector);
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
    std::cout << "PORT: " << _port << std::endl;
    
    setContentDisposition(lineVector);
    std::cout << "DESPUES DE SET\n";
    for (std::vector<std::string>::iterator iter; iter != _ContentDisposition.end(); iter++)
    {
        std::cout << "ContentDisposition is: " << *iter << std::endl;
    }
    // std::cout << _header.at("Content-Disposition") << std::endl;

    return (errorsToken());
    return (0);
}
// crear archivo con filename="Python_Modulo_00_ES.pdf"
// y meter todo lo que viene debajo

// ------WebKitFormBoundary3AFsyzo2QGEeTKWQ
// Content-Disposition: form-data; name="name"

// aa
//  LA KEY ES NAME Y EL VALOR ES aa

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
