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
        throw EmptyFile();
    parse();
}

ConfigFile::~ConfigFile()
{
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
            throw ExtraClosing();
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

void ConfigFile::savePort(std::string &str, std::string &ip, uint32_t &port)
{
    ip = str.substr(0, str.find(':'));
    std::string port_str = str.substr(str.find(':') + 1);
    port = std::stoi(port_str);

    if (port_str.find_first_not_of("0123456789") != std::string::npos)
        throw InvalidValue();
    if (port > 65535)
        throw InvalidValue();
}

void ConfigFile::listen(std::vector<std::string>::iterator &it, Config conf)
{
    std::string str = *it;
    std::string ip = "0.0.0.0";
    uint32_t port = 8000;

    if (str.find(':') != std::string::npos)
        savePort(str, ip, port);
    else if (str.find_first_not_of("0123456789") != std::string::npos)
        ip = str;
    else
        port = std::stoi(str);
    conf.addListen(port);

    // COMPROBACION
    std::cout << "listen: " << port << std::endl;
}

/**
 * @brief SERVER NAME
 * Guardamos el "server name"
 *
 * @param it
 */
void ConfigFile::servername(std::vector<std::string>::iterator &it, Config &conf)
{
    while (*it != ";")
        conf.addServerName(*it++);
    
    // COMPROBACION
    std::vector<std::string> vect = conf.getServerName();
    std::vector<std::string>::iterator iter = vect.begin();
    std::cout << "serverName: " << *iter << std::endl;
}

/**
 * @brief ROOT
 * Guardamos el "root"
 * Después de root solo puede venir una palabra.
 * Si tiene más y no acaba en ";", es incorrecto.
 *
 * @param it
 */
void ConfigFile::root(std::vector<std::string>::iterator &it, Config &conf)
{
    conf.setRoot(*it);
    if (*++it != ";")
        throw InvalidValue();
    
    // COMPROBACION
    std::cout << "root: " << conf.getRoot() << std::endl;
}

/**
 * @brief CGI
 * Guardamos el "cgi"
 * Después de root solo pueden venir dos palabras.
 * Si no hay cgi + 2 palabras + ';', es incorrecto.
 *
 * @param it
 */
void ConfigFile::cgi(std::vector<std::string>::iterator &it, Config &conf)
{
    std::string &ext = *it++;
    std::string &exec = *it++;
    conf.addCgi(ext, exec);
    if (*it != ";")
        throw InvalidValue();

    // COMPROBACION
    std::map<std::string, std::string> mapa = conf.getCgi();
    std::map<std::string, std::string>::iterator iter = mapa.begin();
    std::cout << "cgi: " << iter->first << " | " << iter->second << std::endl;
}

/**
 * @brief INDEX
 * Guardamos el "index".
 * Después de index solo puede venir una palabra.
 * Si no hay index + 1 palabra + ';', es incorrecto.
 *
 * @param it
 */
void ConfigFile::index(std::vector<std::string>::iterator &it, Config &conf)
{
    if (*it != ";")
        conf.addIndex(*it);
    else
        throw InvalidValue();
    if (*(++it) != ";")
        throw InvalidValue();
    
    // COMPROBACION
    std::vector<std::string> vect = conf.getIndex();
    std::vector<std::string>::iterator iter = vect.begin();
    std::cout << "index: " << *iter << std::endl;
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
void ConfigFile::limitExcept(std::vector<std::string>::iterator &it, Config conf)
{
    while (*it != ";")
        conf.addLimitExcept(*it++);
    
    // COMPROBACION
    std::vector<std::string> vect = conf.getLimitExcept();
    std::vector<std::string>::iterator iter = vect.begin();
    std::cout << "limit: " << *iter << std::endl;
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
void ConfigFile::errorPage(std::vector<std::string>::iterator &it, Config conf)
{
    std::vector<int> codes;
    int nbr = 0;

    while (it->find_first_not_of("0123456789") == std::string::npos)
        nbr = std::stoi(*it++);
    conf.addErrorPage(nbr, *it);
    if (*++it != ";")
        throw InvalidValue();

    // COMPROBACION
    std::map<int, std::string> mapa = conf.getErrorPage();
    std::map<int, std::string>::iterator iter = mapa.begin();
    std::cout << "errorPage: " << iter->first << " | " << iter->second << std::endl;
}

/**
 * @brief CLIENT MAX BODY SIZE
 * Checkeamos que client max body size sea solo un número
 *
 * @param it
 */
void ConfigFile::client_max_body_size(std::vector<std::string>::iterator &it, Config conf)
{
    if (it->find_first_not_of("0123456789") != std::string::npos)
        throw InvalidValue();
    conf.setClientMaxBodySize(std::stoi(*it));
    if (*++it != ";")
        throw InvalidValue();
    
    // COMPROBACION
    std::cout << "client_max_body_size: " << conf.getClientMaxBodySize() << std::endl;
}

/**
 * @brief AUTOINDEX
 * procesa las solicitudes que terminan con el carácter de slash (' /')
 * y produce una lista de directorios
 * Tenemos que saber si está en on o off, ninguna otra y que después haya un ';'
 *
 * @param it
 */
void ConfigFile::autoindex(std::vector<std::string>::iterator &it, Config conf)
{
    if (*it == "on")
        conf.setAutoindex(true);
    else if (*it == "off")
        conf.setAutoindex(false);
    else
        throw InvalidValue();
    if (*++it != ";")
        throw InvalidValue();

    // COMPROBACION
    std::cout << "autoindex: " << conf.getAutoindex() << std::endl;
}

/**
 * @brief Miramos si el string es alguno de la lista.
 * Función auxiliar en locationLoop.
 * https://www.thegeekstuff.com/2017/05/nginx-location-examples/
 * Un modificador es opcional y actua como un prefijo para la URL entrante
 * -> = <- fo all server errors Ex.: location = /404.html (coincidencia de expresion regular)
 * -> ~ <- no distingue de mayusculas y minusculas Ex.: location ~* .(png|gif|ico|jpg|jpe?g)$ (coincidencia de prefijo)
 * Hay mas modificadores, como (), | 
 * 
 * TODO ASK ALBERTO : Hacemos el location modifier ????
 *
 * @param str
 * @return true
 * @return false
 */
bool isLocationModifier(std::string &str)
{
    return (str == "=" ||
            str == "~");
}

/**
 * @brief Checkeamos si el modificador que nos han pasado se puede aplicar a nuestro location
 * Si no es válido, mandamos excepción
 * 
 * @param str 
 */
// void	ConfigFile::modificateLocation(std::string &str)
// {
// 	if (str == "=")
// 		_modifier = EXACT;
// 	else if (str == "~")
// 		_modifier = CASE_SENSITIVE_REG;
// 	else
// 		throw InvalidValue();
// }

/**
 * @brief Miramos en bucle lo que tenemos dentro de location
 * Si encontramos algún modificador, checkeamos que sea correcto
 * Enviamos el iterador a checkValidDir, enviandole el nuevo archivo de configuracion
 *
 * @param it
 * @param locations
 */
void ConfigFile::locationLoop(std::vector<std::string>::iterator &it)
{
    if (isLocationModifier(*it))
    {
    // 	modificateLocation(*it);
    	it++;
    }
    // else
    // 	_modifier = NONE;

    Config conf;
    // _uri = *it++;
    while (*(++it) != "}")
    	checkValidDir(it, conf);
    // locations.push_back(*this);
}

/**
 * @brief LOCATIONS
 * Guardamos "locations"
 * Podemos encontrar varios "locations", por lo que vamos a necesitar guardarlos con _locationsloop
 * Location es la ubicación del directorio
 * @param it
 */
void ConfigFile::location(std::vector<std::string>::iterator &it)
{
    locationLoop(it);
};

void ConfigFile::checkValidDir(std::vector<std::string>::iterator &it, Config conf)
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
        location(++it);
    else if (*it == "limit_except")
        limitExcept(++it, conf);
    else if (*it == "error_page")
        errorPage(++it, conf);
    else if (*it == "client_max_body_size")
        client_max_body_size(++it, conf);
    else if (*it == "autoindex")
        autoindex(++it, conf);
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
        throw InvalidValue();
    
    std::cout << "VOY A CREAR EL ARCHIVO DE CONFIGURACION\n";
    Config conf;
    while (*(++itV) != "}")
    {
        checkValidDir(itV, &conf);
    }

    // std::string _root = "/";
    // si hay un location
    // 	addLocation("/data", location); Crea un puntero de un config
    // esto es otro config con sus propias funciones
    // el constructor : Config::Config(Config *parent) : _parent (parent)
    // es cuando tengamos el addlocation, iremos a esa configuracion a darle los valores

    // config.setRoot(_root);
    // serv._configServer(++it);
    // _configs.push_back(configs);
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
        throw InvalidFile();

    while (std::getline(myfile, line))
    {
        _fileContent += line + "\n";
        tmp = Strings::trim(line);
        if (tmp[0] != '#' && tmp.length() > 0)
        {
            openBrackets(brackets, tmp);
            closeBrackets(brackets, tmp);
            if (isValidDirective(tmp) && line[line.length()] != ';')
                throw MissingDot();
            if (tmp.find(';', tmp.length() - 1) != std::string::npos)
                endOfLine(tmp);
            else
                pushToken(tmp);
        }
        line_idx++;
    }
    // for (std::size_t i = 0; i < _token.size(); i++)
    // {
    //     std::cout << "token: " << _token[i] << "\n";
    // }
}

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
            std::cout << "PASO POR AQUI " << i << " veces\n";
            i++;
            configToken(iter);
            iter--;
        }

        // else
        //     throw InvalidBlock();
    }
}

const char *ConfigFile::EmptyFile::what() const throw()
{
    return "Empty file";
}

const char *ConfigFile::ExtraClosing::what() const throw()
{
    return "Extra {";
}

const char *ConfigFile::ExtraOpening::what() const throw()
{
    return "Extra }";
}

const char *ConfigFile::MissingDot::what() const throw()
{
    return "Missing ;";
}

const char *ConfigFile::InvalidBlock::what() const throw()
{
    return "Invalid block";
}

const char *ConfigFile::EmptyServer::what() const throw()
{
    return "Empty server";
}

const char *ConfigFile::InvalidFile::what() const throw()
{
    return "Invalid file";
}

const char *ConfigFile::InvalidValue::what() const throw()
{
    return "Invalid value in the file";
}
