#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "Http/Request.hpp"
#include "Server/Config.hpp"

class CGI
{
private:
    int _fd;
    std::string _pathCgi;
    std::string _pathFile;
    // std::map<std::string, std::string> _env;

    CGI();
    // void initEnviron(Request &request, Config &config);

public:
    CGI(int fd, std::string pathCgi, std::string pathFile);
    ~CGI();
    void execute();
};
