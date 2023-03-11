#pragma once
#include <iostream>
class Utils;

class Log
{
    private:
    std::string _log;

    public:
    static void printLog(std::string method);
};
