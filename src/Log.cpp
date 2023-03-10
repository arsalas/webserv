#include "Log.hpp"
#include "Utils.hpp"
#include "Request.hpp"

std::string    Log::printLog(void)
{
    std::string str;
    size_t time = Utils::getTimeSeconds();
    str = "hola";
    str = str + std::to_string(time);
    // str = str + " method: " + Request::getMethod();
    return (str);
    // time_t  time = getTimeSeconds();
}
