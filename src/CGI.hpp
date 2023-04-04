#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "Http/Request.hpp"

class CGI
{
private:
    std::string _cgiBin;
    std::string _cgiExtension; // .py ej
    std::string _cgiPath;
    std::string _execPath;
    std::string _cwd;
    int _status;
    std::map<std::string, std::string> _env;

    CGI();
    void initEnviron(Request &request, Config &config);
    CGI(std::string bin, std::string extension, std::string path);
    int execute(Request &request, std::string CGIPath, std::string file);

public:
    CGI(Request &request);
    ~CGI();

private:
    /*      GETTERS     */
    std::string getBin();
    std::string getExtension();
    std::string getPath();
};
