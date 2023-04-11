#include <vector>
#include "ConfigFile.hpp"
#include "Utils/File.hpp"
#include "Utils/Strings.hpp"
#include "Server/Config.hpp"

ConfigFile::ConfigFile(std::string path)
{
    File file(path);

    _file = path;
    if (_file.empty())
        throw myException("Empty file", 0);
    parse();
}

std::vector<Config> ConfigFile::getConfigs()
{
    return (_configs);
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::errorConfig(Config *config)
{
    if ((config->getListen()).empty())
        throw myException("No port found", 0);
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
void ConfigFile::closeBrackets(std::stack<bool> &brackets, std::string &tmp)
{
    if (tmp.find('}') < tmp.length())
    {
        if (brackets.empty())
            throw myException("Incorrect use of brackets { }", 0);
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
        _token.push_back(token);
        tmp.erase(0, pos + 1);
    }
    _token.push_back(tmp);
    _token.push_back(";");
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
        _token.push_back(token);
        tmp.erase(0, pos + 1);
    }
    _token.push_back(tmp);
}

/**
 * @brief LISTEN
 * Guardamos el token "listen"
 * Si encontramos ':' guardamos la ip, el puerto y comprobamos que ambos sean correctos
 *
 * @param it
 */
void ConfigFile::savePort(std::string &str, std::string &ip, uint32_t &port)
{
    ip = str.substr(0, str.find(':'));
    std::string port_str = str.substr(str.find(':') + 1);
    port = std::stoi(port_str);

    if (port_str.find_first_not_of("0123456789") != std::string::npos)
        throw myException("Invalid port", 0);
    if (port > 65535)
        throw myException("Invalid port", 0);
}

/**
 * @brief LISTEN
 * Guardamos el token "listen"
 * Si encontramos ':' guardamos la ip, el puerto y comprobamos que ambos sean correctos
 * // TODO CAMBIAR PUERTO Y QUITAR COSAS
 * @param it
 */
void ConfigFile::listen(std::vector<std::string>::iterator &it, Config *conf)
{
    std::string str = *it;
    uint32_t port = std::stoi(str);

    if (str.find_first_not_of("0123456789") != std::string::npos)
        throw myException("Invalid port", 0);
    if (port > 65535)
        throw myException("Invalid port", 0);

    conf->addListen(std::stoi(*it));
    if (*(++it) != ";")
        throw myException("Error in port", 0);
}

/**
 * @brief SERVER NAME
 * Guardamos el "server name"
 *
 * @param it
 */
void ConfigFile::servername(std::vector<std::string>::iterator &it, Config *conf)
{
    while ((*it).empty())
        it++;
    while (*it != ";")
        conf->addServerName(*it++);
}

/**
 * @brief ROOT
 * Guardamos el "root"
 * Después de root solo puede venir una palabra.
 * Si tiene más y no acaba en ";", es incorrecto.
 *
 * @param it
 */
void ConfigFile::root(std::vector<std::string>::iterator &it, Config *conf)
{
    while ((*it).empty())
        it++;
    conf->setRoot(*it);
    if (*++it != ";")
        throw myException("Invalid root", 0);
}

/**
 * @brief CGI
 * Guardamos el "cgi"
 * Después de root solo pueden venir dos palabras.
 * Si no hay cgi + 2 palabras + ';', es incorrecto.
 *
 * @param it
 */
void ConfigFile::cgi(std::vector<std::string>::iterator &it, Config *conf)
{
    std::string &ext = *it++;
    std::string &exec = *it++;
    conf->addCgi(ext, exec);
    if (*it != ";")
        throw myException("Invalid CGI", 0);
}

/**
 * @brief INDEX
 * Guardamos el "index".
 * Después de index solo puede venir una palabra.
 * Si no hay index + 1 palabra + ';', es incorrecto.
 *
 * @param it
 */
void ConfigFile::index(std::vector<std::string>::iterator &it, Config *conf)
{
    if (*it != ";")
        conf->addIndex(*it);
    else
        throw myException("Invalid index", 0);
    if (*(++it) != ";")
        throw myException("Invalid index", 0);
}

/**
 * @brief LIMIT EXCEPT
 * Guardamos el "limitExcept".
 * Después de limitExcept solo puede venir una palabra.
 * Si no hay limitExcept + 1 palabra + ';', es incorrecto.
 * limitExcept nos va a dar la orden: PUT, GET, POST, etc.
 *
 * @param it
 */
void ConfigFile::limitExcept(std::vector<std::string>::iterator &it, Config *conf)
{
    if (*it == ";")
        throw myException("Invalid limit except", 0);
    while (*it != ";")
    {
        if (*it != "POST" && *it != "DELETE" && *it != "GET" && *it != "PUT")
            throw myException("Invalid limit_except", 0);
        conf->addLimitExcept(*it++);
    }
}

/**
 * @brief ERROR_PAGE
 * Guardamos el "error_page"
 * Tienen que dar el código de error y el path
 * El código de error solo pueden ser números
 * Ej.: error_page 404 /my_errors/404.html;
 *
 * @param it
 */
void ConfigFile::errorPage(std::vector<std::string>::iterator &it, Config *conf)
{
    std::vector<int> codes;
    int nbr = 0;

    while (it->find_first_not_of("0123456789") == std::string::npos)
        nbr = std::stoi(*it++);
    conf->addErrorPage(nbr, *it);
    if (*++it != ";")
        throw myException("Invalid error page", 0);
}

/**
 * @brief CLIENT MAX BODY SIZE
 * Checkeamos que client max body size sea solo un número
 *
 * @param it
 */
void ConfigFile::client_max_body_size(std::vector<std::string>::iterator &it, Config *conf)
{
    while ((*it).empty())
        it++;
    if (it->find_first_not_of("0123456789") != std::string::npos)
        throw myException("Invalid client max body size", 0);
    conf->setClientMaxBodySize(std::stoi(*it));
    if (*++it != ";")
        throw myException("Invalid client max body size", 0);
}

/**
 * @brief AUTOINDEX
 * procesa las solicitudes que terminan con el carácter de slash (' /')
 * y produce una lista de directorios
 * Tenemos que saber si está en on o off, ninguna otra y que después haya un ';'
 *
 * @param it
 */
void ConfigFile::autoindex(std::vector<std::string>::iterator &it, Config *conf)
{
    if (*it == "on")
        conf->setAutoindex(true);
    else if (*it == "off")
        conf->setAutoindex(false);
    else
        throw myException("Invalid autoindex", 0);
    if (*++it != ";")
        throw myException("Invalid autoindex", 0);
}

/**
 * @brief Miramos en bucle lo que tenemos dentro de location
 * Si encontramos algún modificador, checkeamos que sea correcto
 * Enviamos el iterador a setTokens, enviandole el nuevo archivo de configuracion
 *
 * @param it
 * @param locations
 */
void ConfigFile::locationLoop(std::vector<std::string>::iterator &it, Config *conf)
{
    Config *location = new Config(conf);
    conf->addLocation(*(it), location);
    while (*(++it) != "}")
        setTokens(it, location);
}

/**
 * @brief LOCATIONS
 * Guardamos "locations"
 * Podemos encontrar varios "locations", por lo que vamos a necesitar guardarlos con _locationsloop
 * Location es la ubicación del directorio
 * @param it
 */
void ConfigFile::location(std::vector<std::string>::iterator &it, Config *conf)
{
    locationLoop(it, conf);
};

/**
 * @brief REWRITE
 *
 * @param it
 * @param conf
 */
void ConfigFile::rewrite(std::vector<std::string>::iterator &it, Config *conf)
{
    while ((*it).empty())
        it++;
    if (*it == ";")
        throw myException("Invalid rewrite", 0);
    conf->setRewrite(*it);
};

/**
 * @brief UPLOAD
 * 
 * @param it 
 * @param conf 
 */
void ConfigFile::upload(std::vector<std::string>::iterator &it, Config *conf)
{
    while ((*it).empty())
        it++;
    if (*it == ";")
        throw myException("Invalid upload", 0);
    conf->setUpload(*it);
    if (*(++it) != ";")
        throw myException("Invalid upload", 0);
};

void ConfigFile::setTokens(std::vector<std::string>::iterator &it, Config *conf)
{
    if (*it == "listen")
        listen(++it, conf);
    else if (*it == "server_name")
        servername(++it, conf);
    else if (*it == "root")
        root(++it, conf);
    else if (*it == "cgi")
        cgi(++it, conf);
    else if (*it == "index")
        index(++it, conf);
    else if (*it == "location")
        location(++it, conf);
    else if (*it == "limit_except")
        limitExcept(++it, conf);
    else if (*it == "error_page")
        errorPage(++it, conf);
    else if (*it == "client_max_body_size")
        client_max_body_size(++it, conf);
    else if (*it == "autoindex")
        autoindex(++it, conf);
    else if (*it == "rewrite")
        rewrite(++it, conf);
    else if (*it == "upload")
        upload(++it, conf);
}

/**
 * @brief El token es "server"
 * Le asignamos un id y hacemos un push_back en _servers
 *
 * @param i
 * @param it
 */
void ConfigFile::configToken(std::vector<std::string>::iterator &iter)
{
    std::vector<std::string>::iterator itV = _configToken.end();

    _configToken.insert(itV, *iter);
    iter++;
    for (; iter != _token.end() && *iter != "server"; iter++)
    {
        itV = _configToken.end();
        _configToken.insert(itV, *iter);
    }
    itV = _configToken.begin();
    itV++;
    if (*itV != "{")
        throw myException("Incorrect use of brackets { }", 0);

    Config conf;
    while (*(++itV) != "}")
    {
        setTokens(itV, &conf);
    }
    _configs.push_back(conf);
    _configToken.clear();
    errorConfig(&conf);
}

void ConfigFile::token()
{
    int line_idx = 1;
    std::string line;
    std::string tmp;
    std::stack<bool> brackets;
    std::vector<int>::iterator iter;
    std::ifstream myfile(_file);

    if (!myfile || !myfile.is_open())
        throw myException("Invalid file", 0);

    while (std::getline(myfile, line))
    {
        _fileContent += line + "\n";
        tmp = Strings::trim(line);
        if (tmp[0] != '#' && tmp.length() > 0)
        {
            openBrackets(brackets, tmp);
            closeBrackets(brackets, tmp);
            if (isValidDirective(tmp) && line[line.length()] != ';')
                throw myException("Missing semicolon ;", 0);
            if (tmp.find(';', tmp.length() - 1) != std::string::npos)
                endOfLine(tmp);
            else
                pushToken(tmp);
        }
        line_idx++;
    }
}

/**
 * @brief Recogemos los tokens
 * Guardamos la información de server y workers en su respectiva estructura de Config
 *
 */
void ConfigFile::parse()
{
    std::vector<std::string>::iterator iter = _token.begin();

    int i = 1;
    token();
    for (iter = _token.begin(); iter != _token.end(); iter++)
    {
        if ((*iter).empty())
            continue;
        if (*iter == "server")
        {
            i++;
            configToken(iter);
            iter--;
        }
    }
}
