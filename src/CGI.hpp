#pragma once

#include <iostream>

class CGI
{
    private:
        std::string _bin;
        std::string _extension; // .py ej
        std::string _path;

        CGI();

    public:
        CGI(std::string bin, std::string extension, std::string path);

        // Tiene que ejecutar el bin con el path
        // fork y execve solo para esto
        // luego se tendra que meter como body en una response
        // *argv[] = {"/usr/bin/python3", "path"}
        // execv(argv[0], argv)
        std::string execute();

        //Alguna excewpcion por si no funciona
};
