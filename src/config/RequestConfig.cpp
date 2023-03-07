#include "RequestConfig.hpp"

RequestConfig::RequestConfig(Request &request, Listen &host_port, std::vector<ServerConfig> &servers, Client &client)
	: _request(request), host_port_(host_port), _client(client), _servers(servers)
{
}

RequestConfig::~RequestConfig()
{
}

void RequestConfig::setup(InputArgs &options)
{
	ServerConfig *server = getServerForRequest(_servers);
	ServerConfig *location = NULL;

	if (_request.getStatus() > 2)
		location = getLocationForRequest(server, _request._target);

	server_ = server;
	_location = server;

	_target = _request._target;

	if (location)
	{
		_location = location;
		if (!options.location() && _request._target.find(location->_uri) != std::string::npos)
			_target.erase(0, _location->_uri.length());
	}
}

void RequestConfig::redirectLocation(std::string target)
{
	ServerConfig *location = NULL;

	if (_request.getStatus() > 2)
		location = getLocationForRequest(server_, target);

	_target = target;
	if (location)
	{
		_location = location;
		if (_target.find(location->_uri) != std::string::npos)
			_target.erase(0, _location->_uri.length());
	}
}

std::vector<ServerConfig *> RequestConfig::matchServer(std::vector<ServerConfig> &servers)
{
	std::vector<ServerConfig *> matching_servers;

	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		for (std::vector<Listen>::iterator list = it->_listens.begin(); list != it->_listens.end(); list++)
		{
			if ((list->_ip == host_port_._ip || list->_ip == "0.0.0.0") && list->port_ == host_port_.port_)
			{
				matching_servers.push_back(&(*it));
				break;
			}
		}
	}
	return (matching_servers);
}

/**
 * @brief Queremo saber qué servidores hacen match con el request
 * Recorremos con dos vectores servers y _listens y cuando haya coincidencia hacemos push_back
 * Si solo encontramos 1 server, lo devolvemos
 * En caso contratio, hacemos un match de servver_name
 * Si no ha encontrado ninguno, utiliza el default
 *
 * @param servers : es el archivo de configuración del server
 * @return ServerConfig*
 */
ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers)
{
	std::vector<ServerConfig *> matching_servers = matchServer(servers);

	// Si solo hay un match, es el nuestro
	if (matching_servers.size() == 1)
		return (matching_servers.front());

	std::string host = _request._headers["Host"].substr(0, _request._headers["Host"].find(':'));

	// Hacemos match en función del basename
	for (std::vector<ServerConfig *>::iterator it = matching_servers.begin(); it != matching_servers.end(); it++)
	{
		std::vector<std::string> server_names = (*it)->getServerNames();

		for (std::vector<std::string>::iterator server_name = server_names.begin(); server_name != server_names.end(); server_name++)
		{
			if (host == *server_name)
				return (*it);
		}
	}
	// Si no nos pasan ninguno, utilizamos el default
	return (matching_servers.front());
}

/**
 * @brief
 * regex_t es una expresión regular que tiene sus propias funciones
 * |= es como un +=
 *
 * @param locations
 * @param target
 * @return ServerConfig*
 */
ServerConfig *RequestConfig::matchRegexp(std::vector<ServerConfig *> &locations, std::string &target)
{
	regex_t reg;

	for (std::vector<ServerConfig *>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		int flag = REG_NOSUB | REG_EXTENDED;

		if ((*it)->_modifier == 3)
			flag |= REG_ICASE;

		int err = regcomp(&reg, (*it)->_uri.c_str(), flag);

		if (err == 0)
		{
			int match = regexec(&reg, target.c_str(), 0, NULL, 0);
			regfree(&reg);
			if (match == 0)
				return (*it);
		}
	}
	return (NULL);
}

/**
 * @brief
 * Recorremos _locations. Si no hay modificadores y _uri es el target, retornamos el iterador
 *
 * @param server
 * @param target
 * @return ServerConfig*
 */
ServerConfig *RequestConfig::getLocationForRequest(ServerConfig *server, std::string &target)
{
	ServerConfig *location = NULL;
	std::vector<ServerConfig *> reg_locations;

	for (std::vector<ServerConfig>::iterator it = server->_locations.begin(); it != server->_locations.end(); it++)
	{
		if (it->_modifier != 2 && it->_modifier != 3)
		{
			if (it->_modifier == 1 && it->_uri == target)
				return &(*it);
			else if (target.find(it->_uri) == 0)
			{
				if (location && location->_uri.length() < it->_uri.length())
					location = &(*it);
				else if (!location)
					location = &(*it);
			}
		}
		else
			reg_locations.push_back(&(*it));
	}

	if (location && location->_modifier == 4)
		return location;

	if (location && !location->_locations.empty())
	{
		for (std::vector<ServerConfig>::iterator it = location->_locations.begin(); it != location->_locations.end(); it++)
		{
			if (it->_modifier == 2 || it->_modifier == 3)
			{
				reg_locations.insert(reg_locations.begin(), &(*it));
			}
		}
	}

	ServerConfig *reg = matchRegexp(reg_locations, target);

	if (reg)
		return (reg);

	return (location);
}

/**
 * @brief
 * Creamos un vector de métodos
 *
 * @param method
 * @return true
 * @return false
 */
bool RequestConfig::methodAccepted(std::string &method)
{
	// std::vector<std::string> methods = _location->_methods;

	if (_location->_methods.empty())
		return (true);
	if (std::find(_location->_methods.begin(), _location->_methods.end(), method) != _location->_methods.end())
		return (true);
	return (false);
}

/*		SETTERS		*/

void RequestConfig::setTarget(std::string target)
{
	_target = target;
}

/*		GETTERS		*/

std::string &RequestConfig::getTarget()
{
	return _target;
}

std::string &RequestConfig::getRequestTarget()
{
	return _request._target;
}

std::string &RequestConfig::getQuery()
{
	return _request.query_string_;
}

std::string &RequestConfig::getHost()
{
	return host_port_._ip;
}

uint32_t &RequestConfig::getPort()
{
	return host_port_.port_;
}

Client &RequestConfig::getClient()
{
	return (_client);
}

std::string &RequestConfig::getRoot()
{
	return (_location->_root);
}

std::string &RequestConfig::getAuth()
{
	return (_location->_credentials);
}

std::string &RequestConfig::getUri()
{
	return (_location->_uri);
}

std::map<std::string, std::string> &RequestConfig::getCGI()
{
	return (_location->cgi_);
}

std::string &RequestConfig::getCGIBin()
{
	return (_location->_cgiBin);
}

size_t &RequestConfig::getClientMaxBodySize()
{
	return (_location->_client_max_body_size);
}

bool RequestConfig::getAutoindex()
{
	return (_location->_autoindex);
}

std::string &RequestConfig::getUpload()
{
	return (_location->_upload);
}

std::vector<std::string> &RequestConfig::getIndexes()
{
	return (_location->_index);
}

std::map<int, std::string> &RequestConfig::getErrorPages()
{
	return (_location->_error_codes);
}

std::vector<std::string> &RequestConfig::getMethods()
{
	return (_location->_methods);
}

std::string &RequestConfig::getBody()
{
	return (_request._req_body);
}

std::string &RequestConfig::getMethod()
{
	return (_request._method);
}

std::string &RequestConfig::getHeader(std::string key)
{
	return (_request._headers[key]);
}

std::map<std::string, std::string, ft::comp> &RequestConfig::getHeaders()
{
	return (_request._headers);
}

std::string &RequestConfig::getProtocol()
{
	return (_request._protocol);
}

/*		AUX		*/

/**
 * @brief Devolvemos el method, target, server y location
 * method es el http
 * target es la URL
 * server es el servidor al que estamos llamando en las coniguraciones
 * location es /
 *
 * @param level
 * @return std::string
 */
std::string RequestConfig::log(LogLevel level)
{
	std::string ret;

	ret = ret + "method: " + getMethod();
	ret = ret + " | location: " + getUri();
	if (level > INFO)
	{
		for (std::map<std::string, std::string>::iterator it = getHeaders().begin(); it != getHeaders().end(); it++)
			ret = ret + "\n" + it->first + ": " + it->second;
	}
	return (ret);
}
