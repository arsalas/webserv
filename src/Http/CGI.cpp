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
    La respuesta no es como un _req, sino que puede ser documentos HTML, GIF, imágenes, etc.

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


CGI::CGI(int fd, std::string pathCgi, std::string pathFile, Request req, Config conf) : _fd(fd),
                                                                           _pathCgi(pathCgi),
                                                                           _pathFile(pathFile),
                                                                           _req(req),
                                                                           _conf(conf)
{
}

CGI::~CGI()
{
}

void CGI::createEnv()
{
    _env["DOCUMENT_ROOT"] = _conf.getRoot();
    _env["HTTP_HOST"] = _req.getHeaders()["Hostname"];
    _env["HTTP_REFERER"] = _req.getHeaders()["Refer"];
    _env["HTTP_USER_AGENT"] = _req.getHeaders()["User-Agent"];
    _env["HTTPS"] = "off";
    _env["REMOTE_PORT"] = _req.getPort();
    _env["REQUEST_METHOD"] = _req.getMethod();
    _env["REQUEST_URI"] = _pathFile;
    _env["SCRIPT_FILENAME"] = _pathCgi;
    _env["SERVER_PORT"] = _req.getPort();
    _env["SERVER_SOFTWARE"] = "Webserver";
}


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
        if (dup2(_fd, 1) < 0)
        {
            throw myException("Error in fd", 0);
        }

        char *argv[3];
        argv[0] = const_cast<char *>(_pathCgi.c_str());
        argv[1] = const_cast<char *>(_pathFile.c_str());
        argv[2] = NULL;

        char **env = new char *[_env.size() + 1];
        int i = 0;
        for (std::map<std::string, std::string>::iterator iter = _env.begin(); iter != _env.end(); iter++)
        {
            std::string str = iter->first + ": " + iter->second;
            env[i] = const_cast<char *>(str.c_str());
            i++;
        }
        env[i] = NULL;
        

        if (execve(_pathCgi.c_str(), argv, env) == -1) // int execve(const char *filename, char *const argv[], char *const envp[]);
            throw myException("Error in execve", 500);
    }

    else
    {
        int status;

        waitpid(pid, &status, WNOHANG);
        std::cout << "PADRE STATUS: " << status << std::endl;
    }
}
