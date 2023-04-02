#include <iostream>
#include <filesystem>
#include "CGI.hpp"
#include <string>

/*
    CGI es una interfaz de puerta de enlace comun.
    Significa que es una interffaz entre el servidor web y los programas escritos
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
   

    http://www.mnuwer.dbasedeveloper.co.uk/dlearn/web/session01.htm
*/

CGI::CGI(Request &request)
{
    initEnviron(request);
}

/**
 * @brief Introducimos en la variable _env toda la info
 * 
 * @param request 
 */
void    CGI::initEnviron(Request request)
{
    std::map<std::string, std::string>	header = request.getHeader();

    _env["METHOD"] = request.getMethod();
    _env["PROTOCOL"] = request.getHttp();
    _env["PORT"] = request.getPort();
    _env["SERVER_NAME"] = header["Hostname"];
    _env["CONTENT_TYPE"] = header["Content-type"];
}


void    CGI::init()
{
    // _cwd = std::filesystem::current_path();
    if (_cwd.empty())
        return ;
    // TODO mirar si cgiBin comienza por '/'
    // si sí comienza:
    if (_cgiBin[0] == '/')
        _cgiPath = _cgiBin + "/" + _cgiExtension;
    else
        _cgiPath = _cwd + _cgiBin + "/" + _cgiExtension;
}
// *argv[] = {"/usr/bin/python3", "path"}
// std::string execute();

/**
 * @brief EJECUTAMOS
 * Hacemos un fork. Estando en el hijo, cambiamos de directorio al dado por el path
 * Cerramos
 * 
 * 
 * @return int 
 */
int CGI::execute()
{
    int     fd[2];
    pid_t   pid = fork();
    // char    *argv[] = {"/usr/bin/python3", "path"}; // HARCODEO

    if (pid == 0) // CHILD
    {
        // if (chdir(_cgiPath.substr(0, _cgiPath.find_last_of('/'))) == -1) // .c_str()
            // return (500);
        close(fd[1]); // cerramos fd[1]
        if (dup2(fd[0], 0) == -1) // creamos una copia del fd, dándole el valor más bajo
            return (500);
        close(fd[0]); // cerramos fd[0]
        // execve(argv[0], argv, _env); // int execve(const char *filename, char *const argv[], char *const envp[]);
    }
    // execute
    // argv = nombre rograma + nombre programa a ejecutar
    // if (_output.size() == 0)
    //     throw std::runtime_error("CGI: no output from cgi");
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