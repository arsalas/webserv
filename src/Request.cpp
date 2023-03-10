#include "Request.hpp"
#include "Utils.hpp"
#include <string>
#include <iostream>
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

std::map<std::string, std::string> mapSplit(std::vector<std::string> auxVector, std::string delimiter)
{
    std::map<std::string, std::string> auxMap;
    std::vector<std::string> newVector;
    std::vector<std::string>::iterator firstIter;
    std::vector<std::string>::iterator secondtIter;

    std::string start;
    std::string end;
    size_t pos = 0;

    std::vector<std::string>::iterator itVector = auxVector.begin();
    auxMap.insert(std::pair<std::string, std::string>(*firstIter, ""));

    itVector++;
    for (; itVector != auxVector.end(); itVector++)
    {
        newVector = vectorSplit(*itVector, ":");
        firstIter = newVector.begin();
        secondtIter = newVector.begin();
        secondtIter++;
        auxMap.insert(std::pair<std::string, std::string>(*firstIter, *secondtIter));
        std::cout << *firstIter << " & " << *secondtIter << std::endl;
    }
    return (auxMap);
}

/**
 * @brief 
 * Pair1 es lo que hay antes de ':'
 * Pair2 es lo que hay después de ':'
 * 
 */
void Request::tokenRequest(void)
{
    std::string file = "DELETE /gfdgdf HTTP/1.1\nuser-agent: Thunder Client (https://www.thunderclient.com)\naccept: */*\nauthorization: 13245687\ncontent-type: application/json\ncontent-length: 23\naccept-encoding: gzip, deflate, br\nHost: 127.0.0.1:3001\nConnection: close\n{\n\"name\": \"Hola\"\n}; ";
    std::string delimiter = "\n";
    std::vector<std::string> lineVector;
    lineVector = vectorSplit(file, delimiter);

    for (std::vector<std::string>::iterator  it =  lineVector.begin(); it < lineVector.end(); it++)
        std::cout << "In my vector is: " << *it << std::endl;

    delimiter = ":";
    std::map<std::string, std::string> map;
    map = mapSplit(lineVector, delimiter);
}

int main ()
{
    Request re;
    re.tokenRequest();
    return (0);
}