#include "Request.hpp"
#include <string>
#include <iostream>

class Request;
/**
 * @brief 
 * Pair1 es lo que hay antes de ':'
 * Pair2 es lo que hay después de ':'
 * 
 */
void Request::tokenRequest(void)
{
    std::string file = "\"DELETE /gfdgdf HTTP/1.1\nuser-agent: Thunder Client (https://www.thunderclient.com)\naccept: */*\nauthorization: 13245687\ncontent-type: application/json\ncontent-length: 23\naccept-encoding: gzip, deflate, br\nHost: 127.0.0.1:3001\nConnection: close\n{\n\"name\": \"Hola\"\n}\"; ";

    std::string pair1;
    std::string pair2;
    std::string delimiter = ":";
    size_t      pos = 0;

    // quiero en un vector hasta el \n
    std::vector<std::string> auxVector;
    std::string aux;
    std::vector<std::string>::iterator  it;
    delimiter = "\n";
    while ((pos = file.find(delimiter)) != std::string::npos)
    {
        aux = file.substr(0, pos);
        std::cout << aux << std::endl;
        auxVector.insert(it, aux);
        aux.erase(0, pos + delimiter.length());
        it++;
    }

    // while ((pos = file.find(delimiter)) != std::string::npos)
    // {
    //     pair1 = file.substr(0, pos);
    //     pair2 = file.substr(0, pos);
    //     _headers.insert(std::pair<std::string, std::string>(pair1, pair2));
    //     std::cout << pair1 << std::endl;
    //     // std::cout << pair2 << std::endl;
    //     file.erase(0, pos + delimiter.length());
    // }
}

int main ()
{
    Request re;
    re.tokenRequest();
    return (0);
}