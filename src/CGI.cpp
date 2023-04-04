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

CGI::CGI(Request &request)
{
    initEnviron(request);
}

CGI::~CGI()
{
}

/**
 * @brief Introducimos en la variable _env toda la info
 * http://flanagan.ugr.es/wap/curso/introcgi.htm#:~:text=CGI%20son%20las%20siglas%20de,la%20programaci%C3%B3n%20de%20p%C3%A1ginas%20interactivas.
 *
 * @param request
 */
void CGI::initEnviron(Request &request)
{
    Config config;
    std::map<std::string, std::string> header = request.getHeaders();

    _env["METHOD"] = request.getMethod();
    _env["PROTOCOL"] = "HTTP/1.1";
    _env["PORT"] = request.getPort();
    _env["SERVER_NAME"] = header["Hostname"];
    _env["SERVER_SOFTWARE"] = "Webserver";
    _env["CONTENT_TYPE"] = header["Content-type"];
    _env["CONTENT_LENGHT"] = config.getClientMaxBodySize();
    _env["REDIRECT_STATUS"] = "200"; // The 200 OK status code means that the request was successful
    // _env["SCRIPT_NAME"] = ; // Nombre del CGI.
    // _env["QUERY_STRING"] = ; // Argumentos pasado al CGI
}

/**
 * @brief EJECUTAMOS
 * Hacemos un fork. Estando en el hijo, cambiamos de directorio al dado por el path
 * Cerramos
 *
 * @return int
 */
int CGI::execute(Request &request, std::string CGIPath, std::string file)
{
    int outputFd[2];
    int inputFd[2];

    _status = 0;

    // create a pipe and puts the file descriptors for the reading and writing ends
    // of the pipe into filedes [0] and filedes [1]
    if (pipe(inputFd) < 0 || pipe(outputFd) < 0)
    {
        _status = 500;
        throw myException("System error in pipe", 500);
        return (500);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        throw myException("System error in fork", 0);
        _status = 500;
        return (500);
    }

    if (pid == 0) // CHILD
    {
        if (dup2(outputFd[1], 1) < 0 || dup2(inputFd[0], 0) < 0) // creamos una copia del fd, dándole el valor más bajo
        {
            close(outputFd[0]); // cerramos fd[1]
            close(outputFd[1]);
            close(inputFd[0]);
            close(inputFd[1]);
            return (500);
        }

        _execPath = File::getExecPath(file);
        if (chdir(_execPath.c_str()) < 0)
            throw myException("Failed in execute CGI method", 0);
        close(outputFd[0]); // cerramos fd[1]
        close(outputFd[1]);
        close(inputFd[0]);
        close(inputFd[1]);

        char *cgi = strdup(CGIPath.c_str()); // TODO ALBERTO, ESTO LO PODEMOS HACER???
        char *argv[] = {cgi, NULL};
        char **env = NULL; // getEnvArray();
        int i = 0;

        for (std::map<std::string, std::string>::iterator iter = _env.begin(); iter != _env.end(); iter++)
        {
            env[i] = const_cast<char *>((iter->first + " " + iter->second).c_str());
            i++;
        }
        execve(argv[0], argv, env); // int execve(const char *filename, char *const argv[], char *const envp[]);
        return (200);
    }
    else
    {
        _env.clear();
        close(inputFd[0]);
        close(outputFd[1]);
        fcntl(outputFd[0], F_SETFL, O_NONBLOCK);
        // fcntl perform the operation on file descriptor passed in argument
        // F_SETFL set the file status flags to the value specified by arg
        // O_NONBLOCK is a property of the open file description
        if (request.getMethod() != "POST")
        {
            close(inputFd[1]);
            throw myException("Failed in execute CGI method", 0);
        }
        else
        {
            fcntl(inputFd[1], F_SETFL, O_NONBLOCK);
            return (inputFd[1]);
        }
    }
    return (1);
}

std::string CGI::getBin()
{
    return (_cgiBin);
}

std::string CGI::getExtension()
{
    return (_cgiExtension);
}

std::string CGI::getPath()
{
    return (_cgiPath);
}