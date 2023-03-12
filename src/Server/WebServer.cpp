#include "WebServer.hpp"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <cstring>

#include "Http/Response.hpp"
#include "Utils/Strings.hpp"

/**
 * @brief Construct a new Web Server:: Web Server object
 * Inicia los diferentes servidores en funcion de la configuracion
 */
WebServer::WebServer()
{
	// TODO cambiar por la configuracion
	for (size_t i = 0; i < 4; i++)
	{
		_servers.insert(_servers.end(), Server("127.0.0.1", 3000 + i));
	}
	constructPoll();
	initPoll();
}

/**
 * @brief Destroy the Web Server:: Web Server object
 */
WebServer::~WebServer()
{
	delete[] _poll;
}

/**
 * @brief Genera la array de los fd de los sockets de cada servidor
 * para generar el poll de conexiones
 */
void WebServer::constructPoll()
{
	_poll = new struct pollfd[_servers.size()];
	for (size_t i = 0; i < _servers.size(); i++)
	{
		_poll[i].fd = _servers[i].getSocketFd();
		_poll[i].events = POLLIN | POLLPRI;
	}
}

/**
 * @brief Inicia el poll de conexiones de los sockets
 */
void WebServer::initPoll()
{
	poll(_poll, _servers.size(), WebServer::timeout);
	recivedPoll();
}

/**
 * @brief Recive una conexion a un puerto gestionado por el poll de conexiones
 *
 */
void WebServer::recivedPoll()
{
	struct sockaddr_in cli_addr;
	char buffer[RECV_BUFFER_SIZE];
	socklen_t clilen = sizeof(cli_addr);
	// AQUI ES CUANDO RECIVE LA PETICION
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_poll[i].revents & POLLIN)
		{
			int newsockfd = accept(_poll[i].fd, (struct sockaddr *)&cli_addr, &clilen);
			if (newsockfd < 0)
				throw AcceptSocketException();
			memset(&buffer, 0, RECV_BUFFER_SIZE);
			int n = recv(newsockfd, buffer, RECV_BUFFER_SIZE, 0);
			if (n == -1)
				throw RecivedSocketException();
			std::cout << "RECIVED: " << buffer;
			sendResponse(newsockfd, _servers[i]);
		}
	}
	initPoll();
}

/**
 * @brief Envia la respuesta al cliente que se ha conectado al servidor
 *
 * @param fd fd del socket del cliente
 * @param i BORRAR
 */
void WebServer::sendResponse(int fd, Server server)
{
	std::string path = "www/index" + Strings::intToString(i) + ".html";
	Response resp(fd);
	resp.status(200);
	int n = resp.sendFile(path);
	if (n == -1)
		throw SendSocketException();
	close(fd);
}

// Exceptions

const char *WebServer::AcceptSocketException::what() const throw()
{
	return "Cannot create socket";
}

const char *WebServer::RecivedSocketException::what() const throw()
{
	return "ERROR on binding";
}

const char *WebServer::SendSocketException::what() const throw()
{
	return "ERROR on binding";
}