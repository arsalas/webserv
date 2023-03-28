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


/**
 * @brief Guardamos el nombre del archivo y la extensión cuando nos envían un documento
 * 
 */
void    Request::setFilename( void )
{
    std::vector<std::string>::iterator iter = _ContentDisposition.begin();
    std::vector<std::string> auxVector;
    std::vector<std::string>::iterator auxIter;

    for(; iter != _ContentDisposition.end(); iter++)
    {
        if ((*iter).find("filename") != std::string::npos)
        {
            auxVector = Strings::vectorSplit(*iter, ";");
            auxIter = auxVector.begin();
            for (; auxIter != auxVector.end(); auxIter++)
            {
                if ((*auxIter).find("filename") != std::string::npos)
                {
                    auxVector = Strings::vectorSplit(*auxIter, "=");
                    iter = auxVector.begin();
                    iter++;
                    auxVector = Strings::vectorSplit(*iter, ".");
                    iter = auxVector.begin();
                    _filename = Strings::deleteQuotes(*iter);
                    iter++;
                    _extension = Strings::deleteQuotes(*iter);
                    break ;
                }
            }
            break ;
        }
    }
}

/**
 * @brief Cuando envían un archivo, debemos crearlo
 * 
 */
void    Request::createFilename( void )
{
    if (!_ContentDisposition.empty())
    {
        // std::ofstream file(_filename + "." + _extension, std::ios::out);
        std::ofstream file((_filename + "." + _extension).c_str(), std::ios_base::in);
        for (std::vector<std::string>::iterator iter = _fileContent.begin(); iter != _fileContent.end(); iter++)
        {
            file << *iter;
            file << "\n";
        }
        file.close();
    }
}

void    Request::setFileContent(std::vector<std::string> lineVector)
{
    std::vector<std::string>::iterator iter = lineVector.begin();
    std::vector<std::string>::iterator cdIter;

    for(; iter != lineVector.end() && iter < lineVector.end(); iter++)
    {
        if (iter != lineVector.end() && iter < lineVector.end())
        {
            if ((*iter).find("filename=") != std::string::npos)
            {
                iter++;
                iter++;
                while (iter != lineVector.end() && (*iter).find("Content-Disposition:") == std::string::npos && (*iter).find("WebKitFormBoundary:") == std::string::npos)
                {
                    cdIter = _fileContent.end(); 
                    _fileContent.insert(cdIter, *iter);
                    iter++;
                }
            }
        }
    }
}

/**
 * @brief Cuando envían un archivo, debemos guardar su contenido
 * 
 * @param lineVector 
 */
void    Request::setContentDisposition(std::vector<std::string> lineVector)
{
    std::vector<std::string>::iterator iter = lineVector.begin();
    std::vector<std::string>::iterator cdIter;

    for(; iter != lineVector.end(); iter++)
    {
        if ((*iter).find("Content-Disposition:") != std::string::npos)
        {
            cdIter = _ContentDisposition.end();
            _ContentDisposition.insert(cdIter, *iter);
        }
    }
}

/**
 * @brief Creamos un token para method, path, header y body
 * 
 */
int Request::tokenRequest(std::string req)
{
    std::vector<std::string> lineVector;

    lineVector = Strings::vectorSplit(req, "\n");
    setMethod(lineVector);
    setFormData(lineVector);
    setHttp(lineVector);
    setPath(lineVector);
    setHeader(lineVector);
    setBody(lineVector);
    setHostPort(lineVector);
    setContentDisposition(lineVector);
    setFileContent(lineVector);
    setFilename();
    createFilename();

    // std::cout << "METHOD: " << _method << std::endl;
    // std::cout << "HTTP: " << _http << std::endl;
    // std::cout << "PATH: " << _path << std::endl;
    // for (std::map<std::string, std::string>::iterator iterMap  = _header.begin(); iterMap != _header.end(); iterMap++)
    // {
    //     std::cout << "MAP: " << iterMap->first << " & " << iterMap->second << std::endl;
    // }
    // for (std::map<std::string, std::string>::iterator iter  = _formData.begin(); iter != _formData.end(); iter++)
    // {
    //     std::cout << "FORMDATA: " << iter->first << " & " << iter->second << std::endl;
    // }
    // std::cout << "BODY: " << _body << std::endl;
    // std::cout << "PORT: " << _port << std::endl;
    // for (std::vector<std::string>::iterator iter = _ContentDisposition.begin(); iter != _ContentDisposition.end(); iter++)
    // {
    //     std::cout << "ContentDisposition is: " << *iter << std::endl;
    // }
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
