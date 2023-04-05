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

#include "Config/ConfigFile.hpp"

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

WebServer::WebServer() : _configPath("config/default.conf")
{
	start();
}
/**
 * @brief Construct a new Web Server:: Web Server object
 * Inicia los diferentes servidores en funcion de la configuracion
 */
WebServer::WebServer(std::string configPath) : _configPath(configPath)
{
	start();
}

/**
 * @brief Destroy the Web Server:: Web Server object
 */
WebServer::~WebServer()
{
}

void WebServer::start()
{
	ConfigFile configFile(_configPath);

	for (size_t i = 0; i < configFile.getConfigs().size(); i++)
	{
		// configFile.getConfigs()[i];
		_servers.push_back(Server(configFile.getConfigs()[i]));
		std::cout << "Congfig " << i << std::endl;
	}

	// TODO cambiar por la configuracion
	// for (size_t i = 0; i < 1; i++)
	// {

	// 	Config conf;
	// 	_servers.push_back(Server(conf));
	// }
	startSockets();
	initPoll();
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
	std::cout << "START PORTS\n";
	for (it = ports.begin(); it != ports.end(); ++it)
	{
		std::cout << "PORT: " << (*it);
		std::cout << "\n";
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(*it);
		// 0.0.0.0 permite todas las conexiones inet_addr() para convertir el string de la ip
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd < 0)
			throw myException("Cannot create socket", 0);
		if (bind(socketFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
			throw myException("ERROR on binding", 0);
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
				std::cout << "RECIV: " << buf << std::endl;
				if (bufSize == -1)
				{
					Log::Error("Error reciv");
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
				else if (bufSize == 0)
				{
					Log::Error("Client close");

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

						try
						{
							Request req(_fdContent[i]);
							Response resp(pollfds[i].fd);
							Controller ctl(_servers, req, resp);
						}
						catch (const std::exception &e)
						{
							std::cerr << e.what() << '\n';
						}

						_fdContent[i] = "";
						// close(pollfds[i].fd);
						// pollfds[i].fd = 0;
						// pollfds[i].events = 0;
						// pollfds[i].revents = 0;
						// _maxLens[i] = 0;
						// // REALOCAR LOS CLIENTES
						// for (int n = i; n < MAX_CLIENTS - 1; n++)
						// {
						// 	pollfds[n].fd = pollfds[n + 1].fd;
						// 	pollfds[n].events = pollfds[n + 1].events;
						// 	pollfds[n].revents = pollfds[n + 1].revents;
						// 	_maxLens[n] = _maxLens[n + 1];
						// }
						// useClient--;
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
				throw myException("Cannot create socket", 0);
			// close(newsockfd);
			while (1)
			{
				std::cout << "reciv\n";
				std::memset(&buffer, 0, RECV_BUFFER_SIZE);
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
	// Autoindex autoindex("www");
	// resp.status(200).render(autoindex.toStr());
	// resp.status(200).attachment(path);
	// resp.render(autoindex.toStr());
	int n = resp.sendFile(path);
	if (n == -1)
		throw myException("ERROR on binding", 0);
	// close(fd);
}
