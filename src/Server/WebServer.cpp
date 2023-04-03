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
#include "Http/Request.hpp"
#include "Utils/Strings.hpp"
#include "Utils/Colors.hpp"
#include "Server/Controller.hpp"

#include "Pages/Autoindex.hpp"
#include "Logs/Log.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Nosotros utilizamos los Stream sockets, que tienen:
 * 	Recepción garantizada de los datos
 * 	No hay concepto de mensajes
 * 	En el dominio de internet se llaman TCP sockets
 *
 * Sockets activo: cliente
 * Sockets pasivo: servidor
 *
 * Socket pasivo:
 * 	socket()
 * 	bind() - asigna IP y puerto
 * 	listen()
 * 	accept()
 * 	read() / write()
 * 	close()
 *
 * Socket activo:
 * 	socket()
 * 	connect() - conecta con IP y puerto
 * 	read() / write()
 * 	close()
 *
 */

/**
 * @brief Construct a new Web Server:: Web Server object
 * Inicia los diferentes servidores en funcion de la configuracion
 */
WebServer::WebServer()
{
	// TODO cambiar por la configuracion
	for (size_t i = 0; i < 1; i++)
	{
		Config conf;
		_servers.push_back(Server(conf));
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

/**
 * @brief Inicia un socket para cada puerto abierto del server
 *
 */
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
		// Backlog = 5, conexiones maximas que pueden estar esperando
		listen(socketFd, 5);
		addPoll(socketFd);
	}
}

/**
 * @brief Inicia el poll de conexiones de los sockets
 */
void WebServer::initPoll()
{
	struct pollfd pollfds[_poll.size() + MAX_CLIENTS];
	std::memset(&pollfds, 0, sizeof(pollfds));
	// Dar los fd del server
	for (size_t i = 0; i < _poll.size(); i++)
	{
		pollfds[i].fd = _poll[i].fd;
		pollfds[i].events = POLLIN | POLLPRI;
	}
	int useClient = 0;
	while (1)
	{
		Log::Info("Waiting on poll...");
		int pollSize = _poll.size() + useClient;
		int res = poll(pollfds, pollSize, WebServer::timeout);
		Log::Success("Recived poll");
		if (res < 0)
		{
			Log::Error("Poll error");
			return;
		}
		// CERRAR CLIENTES SI ALCANZA EL TIMEOUT
		if (res == 0)
		{
			for (int i = _poll.size(); i < MAX_CLIENTS; i++)
			{
				if (pollfds[i].fd == 0)
					continue;
				pollfds[i].fd = 0;
				pollfds[i].events = 0;
				pollfds[i].revents = 0;
				_fdContent[i] = "";
				close(pollfds[i].fd);
				useClient--;
			}
			continue;
		}
		// BUSCAR EL SERVER
		struct sockaddr_in cli_addr;
		std::string content;
		socklen_t clilen = sizeof(cli_addr);
		bool find = false;
		for (size_t i = 0; i < _poll.size(); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				Log::Success("New Client");
				find = true;
				int newsockfd = accept(pollfds[i].fd, (struct sockaddr *)&cli_addr, &clilen);
				for (int i = _poll.size(); i < MAX_CLIENTS; i++)
				{
					if (pollfds[i].fd == 0)
					{
						pollfds[i].fd = newsockfd;
						pollfds[i].events = POLLIN | POLLPRI;
						useClient++;
						break;
					}
				}
			}
		}
		if (find)
			continue;
		// LEER LOS CLIENTES (recorres en array a ver qué fd ha cambiado en el poll)
		find = false;
		for (int i = _poll.size(); i < MAX_CLIENTS; i++)
		{
			if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
			{
				find = true;
				Log::Success("Find Client");
				char buf[RECV_BUFFER_SIZE];
				std::memset(buf, 0, RECV_BUFFER_SIZE);
				int bufSize = recv(pollfds[i].fd, buf, RECV_BUFFER_SIZE, 0); // recv
				if (bufSize == -1)
				{
					Log::Error("Error reciv");
					close(pollfds[i].fd);
					pollfds[i].fd = 0;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
					_fdContent[i] = "";
					useClient--;
				}
				else if (bufSize == 0)
				{
					Log::Error("Client close");
					close(pollfds[i].fd);
					pollfds[i].fd = 0;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
					_fdContent[i] = "";
					useClient--;
				}
				else
				{
					std::string newStr = std::string(buf, bufSize);
					_fdContent[i] += newStr;
					int st = _fdContent[i].find("Content-Length: ");
					// Si tiene content length hay que esperar hasta tener todo el content
					if (st > 0)
					{
						std::string aux = _fdContent[i].substr(st + 16, _fdContent[i].length() - st - 16);
						int end = aux.find("\n");
						_maxLens[i] = std::stoi(aux.substr(0, end));
					}
					if ((st < 0 && bufSize < RECV_BUFFER_SIZE - 1) || (_fdContent[i].length() >= static_cast<size_t>(_maxLens[i])))
					{
						Request req(_fdContent[i]);
						Response resp(pollfds[i].fd);
						Controller ctl(_servers, req, resp);
						_fdContent[i] = "";
						close(pollfds[i].fd);
						pollfds[i].fd = 0;
						pollfds[i].events = 0;
						pollfds[i].revents = 0;
						_maxLens[i] = 0;
						// REALOCAR LOS CLIENTES
						for (int n = i; n < MAX_CLIENTS - 1; n++)
						{
							pollfds[n].fd = pollfds[n + 1].fd;
							pollfds[n].events = pollfds[n + 1].events;
							pollfds[n].revents = pollfds[n + 1].revents;
							_maxLens[n] = _maxLens[n + 1];
						}
						useClient--;
					}
				}
			}
		}
		if (find)
			continue;
	}
}

/**
 * @brief Agrega en el poll un nuevo fd
 *
 * @param fd
 */
void WebServer::addPoll(int fd)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN | POLLPRI;
	_poll.push_back(pfd);
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