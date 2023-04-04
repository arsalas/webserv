#include <iostream>
#include <filesystem>
#include "CGI.hpp"
#include "Server/Config.hpp"
#include "Utils/File.hpp"
#include "Utils/Strings.hpp"
#include <string>

/*
    CGI es una interfaz de puerta de enlace comun (Common Gateway Interface).
    Significa que es una interffaz entre el servidor web y los programas externos
    !!  Permite al servidor web ejecutar un programa externo !!
    CGI recoge los datos del cliente y luego envia una respuesta al cliente
    La respuesta no es como un request, sino que puede ser documentos HTML, GIF, imágenes, etc.

    Cuando un cliente solicita un documento de un servidor, el servidor encuentra el archivo y lo envía al cliente
    Pero si un cliente solicita un programa CGI, el servidor simplemente actúa como intermediario entre el cliente y el programa CGI.

    Proceso:
    (1) El cliente envía una solicitud al servidor
    (2) El servidor crea un proceso CGI
    (3) El proceso CGI convierte la información solicitada en variables de entorno
    (4) Conecta el servidor y un programa CGI externo
    (5) Envía la respuesta al cliente

    La mayoría de los servidores esperan que los programas y scripts CGI residan en un directorio especial,
    generalmente llamado cgi-bin , y/o que tengan una cierta extensión de archivo
    http://www.mnuwer.dbasedeveloper.co.uk/dlearn/web/session01.htm
    https://www.oreilly.com/library/view/cgi-programming-on/9781565921689/04_chapter-01.html#:~:text=With%20CGI%2C%20the%20Web%20server,back%20to%20the%20Web%20browser.
*/

// TODO CGI
// Buscamos que archivo de html hay que mostrar. El CGI es una extension que indicas el programa a ejecutar e indicas la respuesta
// Es un texto generado a traves de un programa
// si tiene ext .py no p
// hay que procesarlo mediante programa

CGI::CGI(int fd, std::string pathCgi, std::string pathFile) : _fd(fd), _pathCgi(pathCgi), _pathFile(pathFile)
{
}

CGI::~CGI()
{
}

// void    CGI::saveEnviron(char **env)
// {

// }

/**
 * @brief Introducimos en la variable _env toda la info
 * http://flanagan.ugr.es/wap/curso/introcgi.htm#:~:text=CGI%20son%20las%20siglas%20de,la%20programaci%C3%B3n%20de%20p%C3%A1ginas%20interactivas.
 *
 * @param request
 */
// void CGI::initEnviron(Request &request, Config &config)
// {
//     std::map<std::string, std::string> header = request.getHeaders();

//     _env["METHOD"] = request.getMethod();
//     _env["PROTOCOL"] = "HTTP/1.1";
//     _env["PORT"] = request.getPort();
//     _env["SERVER_NAME"] = header["Hostname"];
//     _env["SERVER_SOFTWARE"] = "Webserver";
//     _env["CONTENT_TYPE"] = header["Content-type"];
//     _env["CONTENT_LENGHT"] = config.getClientMaxBodySize();
//     _env["REDIRECT_STATUS"] = "200"; // The 200 OK status code means that the request was successful
//     // _env["SCRIPT_NAME"] = ; // Nombre del CGI.
//     // _env["QUERY_STRING"] = ; // Argumentos pasado al CGI
// }

/**
 * @brief EJECUTAMOS
 * Hacemos un fork. Estando en el hijo, cambiamos de directorio al dado por el path
 * Cerramos
 *
 */
void CGI::execute()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        throw myException("System error in fork", 0);
    }

    if (pid == 0) // CHILD
    {
        std::cerr << "CHILD\n";
        if (dup2(_fd, 1) < 0) // creamos una copia del fd, dándole el valor más bajo
        {
            throw myException("Error in fd", 0);
        }

        // char *cgi = (CGIPath.c_str()); // TODO ALBERTO, ESTO LO PODEMOS HACER???
        // char *argv[] = {cgi, NULL};
        // char **env = NULL; // getEnvArray();
        // int i = 0;

        // for (std::map<std::string, std::string>::iterator iter = _env.begin(); iter != _env.end(); iter++)
        // {
        //     env[i] = const_cast<char *>((iter->first + " " + iter->second).c_str());
        //     i++;
        // }
        char *argv[3];
        argv[0] = const_cast<char *>(_pathCgi.c_str());
        argv[1] = const_cast<char *>(_pathFile.c_str());
        argv[2] = NULL;
        if (execve(_pathCgi.c_str(), argv, argv) == -1) // int execve(const char *filename, char *const argv[], char *const envp[]);
            throw myException("Error in execve", 500);
    }

    else
    {
        int status;
    
        waitpid(pid, &status, WNOHANG);
        std::cout << "PADRE STATUS: " << status << std::endl;
        // fcntl(_fd, F_SETFL, O_NONBLOCK);
        // _env.clear();
        // close(inputFd[0]);
        // close(outputFd[1]);
        // fcntl(_fd, F_SETFL, O_NONBLOCK);
        // // fcntl perform the operation on file descriptor passed in argument
        // // F_SETFL set the file status flags to the value specified by arg
        // // O_NONBLOCK is a property of the open file description
        // if (request.getMethod() != "POST")
        // {
        //     close(inputFd[1]);
        //     throw myException("Failed in execute CGI method", 0);
        // }
        // else
        // {
        //     fcntl(inputFd[1], F_SETFL, O_NONBLOCK);
        //     return (inputFd[1]);
        // }
    }
}
