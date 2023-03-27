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
#include "Utils/Colors.hpp"

#include "Pages/Autoindex.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	for (size_t i = 0; i < _poll.size(); i++)
	{
		pollfds[i].fd = _poll[i].fd;
		pollfds[i].events = POLLIN | POLLPRI;
	}

	int useClient = 0;

	while (1)
	{

		std::cout << "size: " << _poll.size() << std::endl;
		int res = poll(pollfds, _poll.size() + useClient, WebServer::timeout);
		// int res = poll(&_poll[0], _poll.size(), WebServer::timeout);
		std::cout << "res: " << res << std::endl;

		if (res < 1)
			return;

		// BUSCAR EL SERVER
		struct sockaddr_in cli_addr;
		// char buf[RECV_BUFFER_SIZE];
		std::string content;
		// int n;
		socklen_t clilen = sizeof(cli_addr);
		for (size_t i = 0; i < _poll.size(); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				std::cout << RED "FIND\n" RESET;
				int newsockfd = accept(_poll[i].fd, (struct sockaddr *)&cli_addr, &clilen);
				std::cout << RED "FD: " << newsockfd << "\n" RESET;

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

		// LEER LOS CLIENTES
		for (int i = _poll.size(); i < MAX_CLIENTS; i++)
		{
			if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
			{
				char buf[RECV_BUFFER_SIZE];
				int bufSize = read(pollfds[i].fd, buf, RECV_BUFFER_SIZE - 1);
				if (bufSize == -1)
				{
					pollfds[i].fd = 0;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
					useClient--;
				}
				else if (bufSize == 0)
				{
					pollfds[i].fd = 0;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
					useClient--;
				}
				else
				{
					printf("From client: %i\n", bufSize);
					buf[bufSize] = '\0';
					// printf("From client: %s\n", buf);
					sendResponse(pollfds[i].fd);
					if (bufSize < RECV_BUFFER_SIZE - 1)
					{

						close(pollfds[i].fd);
						pollfds[i].fd = 0;
						pollfds[i].events = 0;
						pollfds[i].revents = 0;
						useClient--;
						std::cout << GRN "CLOSE\n" RESET;
					}
				}
			}
		}

		// recivedPoll();
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

/**
 * @brief Recive una conexion a un puerto gestionado por el poll de conexiones
 *
 */
void WebServer::recivedPoll()
{
	struct sockaddr_in cli_addr;
	char buffer[RECV_BUFFER_SIZE];
	std::string content;
	int n;
	socklen_t clilen = sizeof(cli_addr);
	for (size_t i = 0; i < _poll.size(); i++)
	{
		std::cout << "FOR\n";
		// TODO? realmente necesito saber de donde viene o me vale con el request
		// TODO* Refactorizar en otra funcion
		if (_poll[i].revents & POLLIN)
		{
			std::cout << "IF\n";
			int newsockfd = accept(_poll[i].fd, (struct sockaddr *)&cli_addr, &clilen);
			printf("accept success %s\n", inet_ntoa(cli_addr.sin_addr));
			std::cout << "newsockfd: " << newsockfd << "\n";

			if (newsockfd < 0)
				throw AcceptSocketException();
			// close(newsockfd);
			while (1)
			{
				std::cout << "reciv\n";
				n = recv(newsockfd, buffer, RECV_BUFFER_SIZE, 0);
				std::cout << GRN "n: " RESET << n << "\n";
				if (n == 0)
				{
					std::cout << RED "END\n";
					close(newsockfd);
				}
				if (n <= 0)
				{
					break;
					return;
				}
				// if (n == -1)
				// 	throw RecivedSocketException();
				content += std::string(buffer);
				// std::bzero(buffer, RECV_BUFFER_SIZE);
				std::memset(&buffer, 0, RECV_BUFFER_SIZE);
				if (n < RECV_BUFFER_SIZE)
					break;
			}

			// TODO parsear request y buscar a donde hay que ir y en que server hay que buscar
			// TODO machear la request con el server y la response
			// Request req(buffer);
			std::cout << "CONTENT:\n"
					  << content << "|" << std::endl;

			std::ifstream file;

			sendResponse(newsockfd);
			break;
		}
	}
}

/**
 * @brief Envia la respuesta al cliente que se ha conectado al servidor
 * @param fd fd del socket del cliente
 */
void WebServer::sendResponse(int fd)
{
	std::string path = "www/index" + Strings::intToString(1) + ".html";
	Response resp(fd);
	// TODO todo esto habra que hacerlo con un controller
	// resp.status(200);
	Autoindex autoindex("www");
	// resp.status(200).render(autoindex.toStr());
	// resp.status(200).attachment(path);
	// resp.render(autoindex.toStr());
	int n = resp.sendFile(path);
	if (n == -1)
		throw SendSocketException();
	// close(fd);
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