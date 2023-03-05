#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : credentials_("off"), autoindex_(false), client_max_body_size_(0), cgi_bin_("cgi-bin")
{
	_modifier = NONE;
	initDirectiveMap();
}

ServerConfig::~ServerConfig()
{
}

ServerConfig &ServerConfig::operator=(const ServerConfig &copy)
{
	client_max_body_size_ = copy.client_max_body_size_;
	_root = copy._root;
	error_codes_ = copy.error_codes_;
	index_ = copy.index_;
	cgi_ = copy.cgi_;
	cgi_bin_ = copy.cgi_bin_;
	autoindex_ = copy.autoindex_;
	credentials_ = copy.credentials_;
	return (*this);
}

/**
 * @brief Guardamos en el array de _directive el parseo de cada una
 * 
 */
std::map<std::string, ServerConfig::type> ServerConfig::_directive;
void ServerConfig::initDirectiveMap()
{
	ServerConfig::_directive["listen"] = &ServerConfig::listen;
	ServerConfig::_directive["location"] = &ServerConfig::location;
	ServerConfig::_directive["server_name"] = &ServerConfig::serverName;
	ServerConfig::_directive["error_page"] = &ServerConfig::error_page;
	ServerConfig::_directive["auth"] = &ServerConfig::auth;
	ServerConfig::_directive["client_max_body_size"] = &ServerConfig::client_max_body_size;
	ServerConfig::_directive["root"] = &ServerConfig::root;
	ServerConfig::_directive["limit_except"] = &ServerConfig::limit_except;
	ServerConfig::_directive["autoindex"] = &ServerConfig::autoindex;
	ServerConfig::_directive["index"] = &ServerConfig::index;
	ServerConfig::_directive["upload"] = &ServerConfig::upload;
	ServerConfig::_directive["cgi"] = &ServerConfig::cgi;
	ServerConfig::_directive["cgi_bin"] = &ServerConfig::cgi_bin;
}

/**
 * @brief SERVER
 * Checkeamos si la palabra "server" del principio del archivo de configuración es correcta.
 * Debe aparecer "{".
 * ALBERTO, no entiendo más allá
 * 
 * @param it 
 */
void ServerConfig::server(std::vector<std::string>::iterator &it)
{
	if (*it != "{")
		throw std::runtime_error("missing opening bracket in server block");
	while (*(++it) != "}")
	{
		// std::cout << "direc: " << *it << std::endl;
		if (ServerConfig::_directive[*it])
			(this->*(ServerConfig::_directive[*it]))(++it);
		else
			throw std::runtime_error("invalid directive '" + *it + "' in 'server'");
	}
}


/**
 * @brief Guardamos la ip y el puerto y checkeamos que sean correctos
 * 
 * @param str 
 * @param ip 
 * @param port 
 */
void	ServerConfig::savePort(std::string &str, std::string &ip, uint32_t &port)
{
	ip = str.substr(0, str.find(':'));
	std::string port_str = str.substr(str.find(':') + 1);
	port = ft::stoi(port_str);

	if (port_str.find_first_not_of("0123456789") != std::string::npos)
	{
		throw std::runtime_error("invalid port :" + port_str);
	}
	if (port > 65535)
		throw std::runtime_error("duplicate value in 'listen'");
}

/**
 * @brief Guardamos el puerto y la IP dentro de _listens si no tiene valores repetidos
 * 
 * @param it 
 * @param ip 
 * @param port 
 */
void	ServerConfig::saveListen(std::vector<std::string>::iterator &it, std::string &ip, uint32_t &port)
{
	Listen	list(ip, port);

	if (std::find(_listens.begin(), _listens.end(), list) != _listens.end())
		throw std::runtime_error("duplicate value in 'listen'");
	_listens.push_back(Listen(ip, port));
	if (*++it != ";")
		throw std::runtime_error("double value in 'listen'");
}

/**
 * @brief LISTEN
 * Guardamos el token "listen"
 * Si encontramos ':' guardamos la ip, el puerto y comprobamos que ambos sean correctos
 * 
 * @param it 
 */
void	ServerConfig::listen(std::vector<std::string>::iterator &it)
{
	std::string	str = *it;
	std::string	ip = "0.0.0.0";
	uint32_t	port = 8000;

	if (str.find(':') != std::string::npos)
		savePort(str, ip, port);
	else if (str.find_first_not_of("0123456789") != std::string::npos)
		ip = str;
	else
		port = ft::stoi(str);
	saveListen(it, ip, port);
}


/**
 * @brief SERVER NAME
 * Guardamos el "server name"
 * 
 * @param it 
 */
void	ServerConfig::serverName(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
		server_name_.push_back(*it++);
}


/**
 * @brief ROOT
 * Guardamos el "root"
 * Después de root solo puede venir una palabra.
 * Si tiene más y no acaba en ";", es incorrecto.
 * 
 * @param it 
 */
void	ServerConfig::root(std::vector<std::string>::iterator &it)
{
	_root = *it;
	if (*++it != ";")
		throw std::runtime_error("double value in 'root'");
};


/**
 * @brief CGI
 * Guardamos el "cgi"
 * Después de root solo pueden venir dos palabras.
 * Si no hay cgi + 2 palabras + ';', es incorrecto.
 * 
 * @param it 
 */
void	ServerConfig::cgi(std::vector<std::string>::iterator &it)
{
	std::string &ext = *it++;
	std::string &exec = *it++;
	cgi_[ext] = exec;
	if (*it != ";")
		throw std::runtime_error("triple value in 'cgi'");
};


/**
 * @brief INDEX
 * Guardamos el "index".
 * Después de index solo puede venir una palabra.
 * Si no hay index + 1 palabra + ';', es incorrecto.
 * 
 * @param it 
 */
void	ServerConfig::index(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
		index_.push_back(*it++);
};

/** ALBERTO, NO HE VISTO NADA QUE PONGA CGI_BIN, NO SE QUE ES ESTO **/
void ServerConfig::cgi_bin(std::vector<std::string>::iterator &it)
{
	cgi_bin_ = *it;
	if (*++it != ";")
		throw std::runtime_error("double value in 'cgi_bin'");
};


/**
 * @brief LOCATIONS
 * Guardamos "locations"
 * Podemos encontrar varios "locations", por lo que vamos a necesitar guardarlos con _locationsloop
 * 
 * @param it 
 */
void	ServerConfig::location(std::vector<std::string>::iterator &it)
{
	ServerConfig loc;

	loc = *this;
	loc.locationLoop(it, _locations);
};


/**
 * @brief Miramos si el string es alguno de la lista.
 * Función auxiliar en locationLoop.
 * 
 * @param str 
 * @return true 
 * @return false 
 */
bool	ServerConfig::isLocationModifier(std::string &str)
{
	return (str == "=" ||
		str == "~" ||
		str == "~*" ||
		str == "^~");
}

/**
 * @brief Checkeamos si el modificador que nos han pasado se puede aplicar a nuestro location
 * Si no es válido, mandamos excepción
 * 
 * @param str 
 */
void	ServerConfig::modificateLocation(std::string &str)
{
	if (str == "=")
		_modifier = EXACT;
	else if (str == "~")
		_modifier = CASE_SENSITIVE_REG;
	else if (str == "~*")
		_modifier = CASE_INSENSITIVE_REG;
	else if (str == "^~")
		_modifier = LONGEST;
	else
		throw std::runtime_error("unknown modifier in location");
}

void	ServerConfig::checkValidDir(std::vector<std::string>::iterator &it)
{
	if (ServerConfig::_directive[*it])
		(this->*(ServerConfig::_directive[*it]))(++it);
	else
		throw std::runtime_error("invalid directive '" + *it + "' in 'location'");
}

/**
 * @brief Miramos en bucle lo que tenemos dentro de location
 * Si encontramos algún modificador, checkeamos que sea correcto
 * 
 * @param it 
 * @param locations 
 */
void	ServerConfig::locationLoop(std::vector<std::string>::iterator &it, std::vector<ServerConfig> &locations)
{
	if (isLocationModifier(*it))
	{
		modificateLocation(*it);
		it++;
	}
	else
		_modifier = NONE;
	_uri = *it++;
	if (*it != "{")
		throw std::runtime_error("missing opening bracket in server block");
	while (*(++it) != "}")
		checkValidDir(it);
	locations.push_back(*this);
}


/**
 * @brief LIMIT EXCEPT
 * Guardamos el "limit_except".
 * Después de limit_except solo puede venir una palabra.
 * Si no hay limit_except + 1 palabra + ';', es incorrecto.
 * 
 * @param it 
 */
void	ServerConfig::limit_except(std::vector<std::string>::iterator &it)
{
	while (*it != ";")
		methods_.push_back(*it++);
};


/**
 * @brief UPLOAD
 * Guardamos el "upload".
 * Después de upload solo puede venir una palabra.
 * Si no hay upload + 1 palabra + ';', es incorrecto.
 * 
 * @param it 
 */
void	ServerConfig::upload(std::vector<std::string>::iterator &it)
{
	upload_ = *it;
	if (*++it != ";")
		throw std::runtime_error("double value in 'upload'");
};


/**
 * @brief CLIENT MAX BODY SIZE
 * Checkeamos que client max body size sea solo un número
 * 
 * @param it 
 */
void	ServerConfig::client_max_body_size(std::vector<std::string>::iterator &it)
{
	if (it->find_first_not_of("0123456789") != std::string::npos)
		throw std::runtime_error("unexpected symbols in client_max_body_size");
	client_max_body_size_ = ft::stoi(*it);
	if (*++it != ";")
		throw std::runtime_error("double value in 'client_max_body_size_'");
};


void	ServerConfig::error_page(std::vector<std::string>::iterator &it)
{
	std::vector<int> codes;

	while (it->find_first_not_of("0123456789") == std::string::npos)
	{
		codes.push_back(ft::stoi(*it++));
	}
	for (std::vector<int>::iterator it2 = codes.begin(); it2 != codes.end(); it2++)
	{
		error_codes_[*it2] = *it;
	}
	if (*++it != ";")
		throw std::runtime_error("double value in 'listen'");
};

void	ServerConfig::auth(std::vector<std::string>::iterator &it)
{
	credentials_ = *it;
	if (*++it != ";")
		throw std::runtime_error("double value in 'auth'");
};

void	ServerConfig::autoindex(std::vector<std::string>::iterator &it)
{
	if (*it == "on")
		autoindex_ = true;
	else if (*it == "off")
		autoindex_ = false;
	else
		throw std::runtime_error("unknown value in 'autoindex'");

	if (*++it != ";")
		throw std::runtime_error("double value in 'autoindex'");
};



/*
** Getter Functions
*/

std::string &ServerConfig::getUri()
{
	return _uri;
}

std::vector<Listen> &ServerConfig::getListens()
{
	return _listens;
};

std::vector<std::string> &ServerConfig::getServerNames()
{
	return server_name_;
};

std::vector<ServerConfig> &ServerConfig::getLocations()
{
	return _locations;
};
