#include <iostream>
#include "Log.hpp"
#include "Utils.hpp"
#include "Request.hpp"
#include "Colors.hpp"

void    Log::printLog(std::string method)
{
    time_t time = Utils::getTimeSeconds();
    std::string str = std::to_string(time);

    std::cout << YEL "[ " << str << " ] " RESET << RED " [ " << method << " ] " RESET << std::endl;
}
