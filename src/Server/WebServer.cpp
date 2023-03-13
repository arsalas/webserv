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
	for (size_t i = 0; i < 1; i++)
	{
		Config conf(true);
		_servers.insert(_servers.end(), Server(conf));
	}
	startSockets();
	initPoll();
}

/**
 * @brief Destroy the Web Server:: Web Server object
 */
WebServer::~WebServer()
{
}

/**
 * @brief Obtiene los diferentes puertos que usaran los servers
 *
 * @return std::set
 */
std::set<int> WebServer::getPorts()
{
	std::set<int> ports;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::vector<int> serverPorts = _servers[i].getPorts();
		for (size_t j = 0; j < serverPorts.size(); j++)
		{
			ports.insert(serverPorts[j]);
		}
	}
	return ports;
}

void WebServer::startSockets()
{
	std::set<int> ports = getPorts();
	std::set<int>::iterator it;
	for (it = ports.begin(); it != ports.end(); ++it)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(*it);
		// 0.0.0.0 permite todas las conexiones inet_addr() para convertir el string de la ip
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd < 0)
			throw CreateSocketException();
		if (bind(socketFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
			throw BindingException();
		listen(socketFd, 5);
		// TODO* refactorizar en otra funcion
		// Inserta el fd en el poll
		struct pollfd pfd;
		pfd.fd = socketFd;
		pfd.events = POLLIN | POLLPRI;
		_poll.push_back(pfd);
	}
}

/**
 * @brief Inicia el poll de conexiones de los sockets
 */
void WebServer::initPoll()
{
	poll(&_poll[0], _poll.size(), WebServer::timeout);
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
	for (size_t i = 0; i < _poll.size(); i++)
	{
		// TODO? realmente necesito saber de donde viene o me vale con el request
		// TODO* Refactorizar en otra funcion
		if (_poll[i].revents & POLLIN)
		{
			int newsockfd = accept(_poll[i].fd, (struct sockaddr *)&cli_addr, &clilen);
			if (newsockfd < 0)
				throw AcceptSocketException();
			std::memset(&buffer, 0, RECV_BUFFER_SIZE);
			int n = recv(newsockfd, buffer, RECV_BUFFER_SIZE, 0);
			if (n == -1)
				throw RecivedSocketException();
			// TODO parsear request y buscar a donde hay que ir y en que server hay que buscar
			// TODO machear la request con el server y la response
			std::cout << "RECIVED: " << buffer << std::endl;
			sendResponse(newsockfd);
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
void WebServer::sendResponse(int fd)
{
	std::string path = "www/index" + Strings::intToString(1) + ".html";
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

const char *WebServer::CreateSocketException::what() const throw()
{
	return "Cannot create socket";
}
const char *WebServer::BindingException::what() const throw()
{
	return "ERROR on binding";
}