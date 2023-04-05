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
    Request _req;
    Config _conf;
    std::map<std::string, std::string> _env;

private:
    CGI();
    void createEnv();

public:
    CGI(int fd, std::string pathCgi, std::string pathFile, Request req, Config conf);
    ~CGI();
    void execute();
};
