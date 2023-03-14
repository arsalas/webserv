#include <iostream>
#include <filesystem>
#include "CGI.hpp"

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
    _env["CONTENT_TYPE"] = header["content-type"];
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