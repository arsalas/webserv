#include "Config.hpp"
#include <algorithm>
#include <cctype>
#include <locale>

Config::Config(std::string &path) : _path(path)
{
	workers_ = 0;
	fd_ = open(_path.c_str(), O_RDONLY);
	if (fd_ < 0)
		throw webserv_exception("could not open configuration file : %", 0, _path);
}

Config::~Config()
{
	clear();
}

void Config::clear()
{
	if (fd_ > 0)
	{
		close(fd_);
		fd_ = 0;
	}
	_tokens.clear();
	file_content_.clear();
}

std::string Config::ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t");
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Config::rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(" \t");
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Config::trim(const std::string &s)
{
	return (rtrim(ltrim(s)));
}

/**
 * @brief Encontramos un ';', por lo que es final de linea
 * 
 * @param tmp 
 */
void Config::endOfLine(std::string &tmp)
{
	tmp.erase(tmp.length() - 1, 1);
	size_t pos = 0;
	std::string token;
	while ((pos = tmp.find(" ")) != std::string::npos)
	{
		token = tmp.substr(0, pos);
		_tokens.push_back(token);
		tmp.erase(0, pos + 1);
	}
	_tokens.push_back(tmp);
	// std::cout << "token: ->" << tmp << "<-" << std::endl;
	_tokens.push_back(";");
}

/**
 * @brief Split por palabras y push del token
 * 
 * @param tmp 
 */
void	Config::pushToken(std::string &tmp)
{
	size_t pos = 0;
	std::string token;

	if (!(tmp.find(' ') < tmp.length()))
	{
	}
	while ((pos = tmp.find(" ")) != std::string::npos)
	{
		token = tmp.substr(0, pos);
		_tokens.push_back(token);
		tmp.erase(0, pos + 1);
	}
	_tokens.push_back(tmp);
}

/**
 * @brief Si encontramos "{", hacemos un push de true
 * 
 * @param brackets 
 * @param tmp 
 */
void	Config::openBrackets(std::stack<bool> &brackets, std::string &tmp)
{
	if (tmp.find('{') < tmp.length())
		brackets.push(true);
}

/**
 * @brief Si encontramos "}", checkeamos si está vacío
 * Y hacemos pop de brackets para saber si al final del 
 * archivo se cierran todos los brackets abiertos
 * 
 * @param brackets 
 * @param tmp 
 * @param line_idx 
 */
void	Config::closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx)
{
	if (tmp.find('}') < tmp.length())
	{
		if (brackets.empty())
			throw webserv_exception("extra closing '}' on line %", 0, ft::to_string(line_idx));
		brackets.pop();
	}
}

/**
 * @brief
 * Abrir file. Leer linea a linea. Trim. No tener en cuenta los comentarios.
 * Check si se ha abierto o cerrado {}.
 * Check si el string es correcto (isValidDirective) y no es final de linea (;).
 * Check si es final de linea (;).
 * Split por palabras. Push dentro de _tokens.
 *
 */
void Config::tokenize()
{
	std::string line, tmp;
	std::stack<bool> brackets;
	std::vector<int>::iterator it;

	int line_idx = 1;

	std::ifstream myfile(_path.c_str());

	while (std::getline(myfile, line))
	{
		file_content_ += line + "\n";
		tmp = trim(line);
		if (tmp[0] != '#' && tmp.length() > 0)
		{
			openBrackets(brackets, tmp);
			closeBrackets(brackets, tmp, line_idx);
			if (isValidDirective(tmp) && line[line.length()] != ';')
				throw webserv_exception("missing ';' on line %", 0, ft::to_string(line_idx));
			if (tmp.find(';', tmp.length() - 1) != std::string::npos)
				endOfLine(tmp);
			else
				pushToken(tmp);
		}
		line_idx++;
	}
}

/**
 * @brief El token es "server"
 * Le asignamos un id y hacemos un push_back en _servers
 * 
 * @param i 
 * @param it 
 */
void	Config::serverToken(int i, std::vector<std::string>::iterator &it)
{
	ServerConfig serv;

	serv.id_ = i++;
	serv.server(++it);
	_servers.push_back(serv);
}

/**
 * @brief El token es "workers"
 * Workers tiene que estar entre 0 y 16 y tiene que acabar en ';'
 * Un service worker es un servicio intermedio entre nuestro navegador 
 * e internet que actúa a modo de proxy y que intercepta toda la 
 * comunicación que se produce entre el dispositivo y la red
 * Básicamente es para que funcione la web sin conexión, utiliza el contenido de caché
 * 
 * @param it 
 */
void	Config::workersToken(std::vector<std::string>::iterator &it)
{
	workers_ = ft::stoi(*(++it));
	if (workers_ > 16 || workers_ < 0)
		throw std::runtime_error("workers must be between [0-16]range");
	if (*++it != ";")
		throw std::runtime_error("missing ';' in 'workers'");
}

/**
 * @brief Recogemos los tokens
 * Guardamos la información de server y workers en su respectiva estructura
 * 
 */
void Config::parse()
{
	int i = 1;
	std::vector<std::string>::iterator it;

	tokenize();
	for (it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		if (*it == "server")
			serverToken(i, it);
		else if (*it == "workers")
			workersToken(it);
		else
			throw webserv_exception("invalid directive % in main block", 0, *it);
	}
	if (_servers.empty())
		throw webserv_exception("missing server block");
}

std::string &Config::getPath()
{
	return (_path);
}

std::vector<ServerConfig> &Config::getServers()
{
	return (_servers);
}

int Config::getWorkers()
{
	return (workers_);
}

std::string &Config::getFileContent()
{
	return (file_content_);
}
