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
	_env["REDIRECT_STATUS"] = "200";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["DOCUMENT_ROOT"] = _conf.getRoot();
	_env["HTTP_HOST"] = Strings::trim(_req.getHeaders()["Host"], " ");
	_env["HTTP_REFERER"] = _req.getHeaders()["Refer"];
	_env["PATH_INFO"] = "/" + _req.getPath();
	_env["HTTP_USER_AGENT"] = _req.getHeaders()["User-Agent"];
	_env["HTTPS"] = "off";
	_env["REMOTE_PORT"] = _req.getPort();
	_env["REQUEST_METHOD"] = _req.getMethod();
	_env["REQUEST_URI"] = "/" + _req.getPath();
	_env["SCRIPT_NAME"] = _pathCgi;
	_env["SCRIPT_FILENAME"] = _pathCgi;
	_env["SERVER_PORT"] = _req.getPort();
	_env["SERVER_SOFTWARE"] = "Webserver";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
}

/**
 * @brief EJECUTAMOS
 * Hacemos un fork. Estando en el hijo, cambiamos de directorio al dado por el path
 * Cerramos
 *
 */
void CGI::execute()
{
	createEnv();
	char *argv[3];
	argv[0] = const_cast<char *>(_pathCgi.c_str());
	argv[1] = const_cast<char *>(_pathFile.c_str());
	argv[2] = NULL;

	// char *env[12];
	char **env = new char *[this->_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator iter = _env.begin(); iter != _env.end(); iter++)
	{
		std::string element = iter->first + "=" + iter->second;
		env[i] = new char[element.size() + 1];
		env[i] = strcpy(env[i], (const char *)element.c_str());
		printf("%s\n", env[i]);
		i++;
	}
	env[i] = NULL;
	std::cout << argv[0] << " " << argv[1] << std::endl;
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

		if (execve(_pathCgi.c_str(), argv, env) == -1)
			throw myException("Error in execve", 500);
	}

	else
	{
		// int status;

		// waitpid(pid, &status, WNOHANG);
		waitpid(pid, NULL, 0);
		// std::cout << "PADRE STATUS: " << status << std::endl;
		std::cout << "PADRE FIN: " << std::endl;
		for (size_t i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
}
