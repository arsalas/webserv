#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <locale>
#include <fstream>
#include <iostream>
#include "Utils/Strings.hpp"
#include "ConfigFile.hpp"

ConfigFile::ConfigFile(std::string &path) : _path(path)
{
    _workers = 0;
    _fd = open(_path.c_str(), O_RDONLY);
    if (_fd < 0)
        throw config_exception("Can't open configuration file : %", 0, _path);
    parse();
}

ConfigFile::~ConfigFile()
{

    // ConfigFile conf;
    // conf.addListen(5000);
    // conf.addListen(5000);
    // conf.addListen(5000);
    // conf.addListen(5000);
    // conf.addListen(5000);
    // Server(conf)


    clear();
}

void ConfigFile::clear()
{
    if (_fd > 0)
    {
        close(_fd);
        _fd = 0;
    }
    _tokens.clear();
    _fileContent.clear();
}

/**
 * @brief Encontramos un ';', por lo que es final de linea
 *
 * @param tmp
 */
void ConfigFile::endOfLine(std::string &tmp)
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
    _tokens.push_back(";");
}

/**
 * @brief Split por palabras y push del token
 *
 * @param tmp
 */
void ConfigFile::pushToken(std::string &tmp)
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
void ConfigFile::openBrackets(std::stack<bool> &brackets, std::string &tmp)
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
void ConfigFile::closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx)
{
    if (tmp.find('}') < tmp.length())
    {
        if (brackets.empty())
            throw config_exception("extra closing '}' on line %", 0, Strings::intToString(line_idx));
        brackets.pop();
    }
}

/**
 * @brief Check si el string enviado es alguno de los mencionados abajo
 *
 * @param str
 * @return true
 * @return false
 */
bool ConfigFile::isValidDirective(std::string const &str)
{
    return (str == "listen" ||
            str == "server_name" ||
            str == "root" ||
            str == "auth" ||
            str == "error_page" ||
            str == "upload" ||
            str == "autoindex" ||
            str == "index" ||
            str == "cgi" ||
            str == "cgi_bin");
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
void ConfigFile::tokenize()
{
    std::string line, tmp;
    std::stack<bool> brackets;
    std::vector<int>::iterator it;
    int line_idx = 1;
    std::ifstream myfile(_path.c_str());

    while (std::getline(myfile, line))
    {
        _fileContent += line + "\n";
        tmp = Strings::trim(line);
        if (tmp[0] != '#' && tmp.length() > 0)
        {
            openBrackets(brackets, tmp);
            closeBrackets(brackets, tmp, line_idx);
            if (isValidDirective(tmp) && line[line.length()] != ';')
                throw config_exception("missing ';' on line %", 0, Strings::intToString(line_idx));
            if (tmp.find(';', tmp.length() - 1) != std::string::npos)
                endOfLine(tmp);
            else
                pushToken(tmp);
        }
        line_idx++;
    }

    for (std::size_t i = 0; i < _tokens.size(); i++)
    {
        std::cout << "token: " << _tokens[i] << "\n";
    }
}

/**
 * @brief El token es "server"
 * Le asignamos un id y hacemos un push_back en _servers
 *
 * @param i
 * @param it
 */
void ConfigFile::serverToken(int i, std::vector<std::string>::iterator &it)
{
    Server serv;

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
void ConfigFile::workersToken(std::vector<std::string>::iterator &it)
{
    _workers = std::stoi(*(++it));
    if (_workers > 16 || _workers < 0)
        throw std::runtime_error("workers must be between [0-16]range");
    if (*++it != ";")
        throw std::runtime_error("missing ';' in 'workers'");
}

/**
 * @brief Recogemos los tokens
 * Guardamos la información de server y workers en su respectiva estructura
 *
 */
void ConfigFile::parse()
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
            throw config_exception("invalid directive % in main block", 0, *it);
    }
    if (_servers.empty())
        throw config_exception("missing server block");
}

/*		GETTERS		*/
std::string &ConfigFile::getPath()
{
    return (_path);
}

std::vector<Server> &ConfigFile::getServers()
{
    return (_servers);
}

int ConfigFile::getWorkers()
{
    return (_workers);
}

std::string &ConfigFile::getFileContent()
{
    return (_fileContent);
}
