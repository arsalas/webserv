#include <iostream>
#include "Log.hpp"
#include "Http/Request.hpp"
#include "Utils/Colors.hpp"

void    Log::printLog(std::string method)
{
    std::cout << RED " [ " << method << " ] " RESET << std::endl;
}

// void    success(std::string method std::string message)
// {

// }
