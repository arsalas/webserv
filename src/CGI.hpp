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
        int         _status;
        std::map<std::string, std::string>  _env;

        CGI(Request &request);
        void    initEnviron(Request &request);

    public:
        CGI(std::string bin, std::string extension, std::string path);
        void    init();
        int     execute(Request &request, std::string CGIPath, std::string path);

        // Tiene que ejecutar el bin con el path
        // fork y execve solo para esto
        // luego se tendra que meter como body en una response
        // *argv[] = {"/usr/bin/python3", "path"}
        // execve(argv[0], argv, env)
        // std::string execute();

    private:
    /*      GETTERS     */
        std::string getBin();
        std::string getExtension();
        std::string getPath();
        //Alguna excepcion por si no funciona
};

// extension path 