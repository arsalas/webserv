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
{}

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

/**
 * @brief El token es "server"
 * Le asignamos un id y hacemos un push_back en _servers
 *
 * @param i
 * @param it
 */
void ConfigFile::serverToken(std::vector<std::string>::iterator &it)
{
    Config config;

    std::cout << "it is:" << *(++it) << std::endl;

    std::string _root = "/";
    // si hay un location
    // 	addLocation("/data", location); Crea un puntero de un config
    // esto es otro config con sus propias funciones
    // el constructor : Config::Config(Config *parent) : _parent (parent)
    // es cuando tengamos el addlocation, iremos a esa configuracion a darle los valores

    config.setRoot(_root);
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
    std::cout << "file is: " << _file << std::endl;

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
    // while (std::getline(myfile, line))
    // {
    //     std::cout << "INSIDE THE WHILE\n";
    //     _fileContent += line + "\n";
    //     tmp = Strings::trim(line);
    //     if (tmp[0] != '#' && tmp.length() > 0)
    //     {
    //         openBrackets(brackets, tmp);
    //         closeBrackets(brackets, tmp);
    //         if (isValidDirective(tmp) && line[line.length()] != ';')
    //             throw MissingDot();
    //         if (tmp.find(';', tmp.length() - 1) != std::string::npos)
    //             endOfLine(tmp);
    //         else
    //             pushToken(tmp);
    //     }
    //     line_idx++;
    // }
    for (std::size_t i = 0; i < _token.size(); i++)
    {
        std::cout << "token: " << _token[i] << "\n";
    }
}

void ConfigFile::parse()
{
    std::vector<std::string>::iterator iter;

    token();
    if (_token.empty())
    for (iter = _token.begin(); iter != _token.end(); iter++)
    {
        std::cout << "TOKEN IS: " << *iter << std::endl;
        if (*iter == "server")
            serverToken(iter);
        else
            throw InvalidBlock();
    }
}

const char *ConfigFile::EmptyFile::what() const throw()
{
	return "Empty file";
}

const char *ConfigFile::ExtraClosing::what() const throw()
{
	return "Extra closing }";
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