#include "Config.hpp"
#include "Http/Request.hpp"
#include <vector>

Config::Config() : _parent(NULL)
{
	// std::vector<int>::iterator ints = _listen.begin();
	// std::vector<std::string>::iterator strs = _serverName.begin();
	// std::map<std::string, std::string>::iterator maps = _cgi.begin();
	// std::map<int, std::string>::iterator error = _errorPage.begin();

	// std::cout << "hola\n";
	// if (ints != _listen.end())
	// 	std::cout << "LISTEN: " << *ints << std::endl;
	// if (strs != _serverName.end())
	// 	std::cout << "SERVNAME: " << *strs << std::endl;
	// if (!_root.empty())
	// 	std::cout << "ROOT: " << _root << std::endl;
	// if (maps != _cgi.end())
	// 	std::cout << "CGI: " << maps->first << " | " << maps->second << std::endl;
	// strs = _index.begin();
	// if (strs != _index.end())
	// 	std::cout << "INDEX: " << *strs << std::endl;
	// // std::cout << "LOCATION: " << getLocation() << std::endl;
	// strs = _limitExcept.begin();
	// if (strs != _limitExcept.end())
	// 	std::cout << "LIMEXP: " << *strs << std::endl;
	// if (error != _errorPage.end())
	// 	std::cout << "ERRORPG: " << error->first << " | " << error->second << std::endl;
	// std::cout << "A.I.: " << getAutoindex() << std::endl;
	// std::cout << "SIZE: " << getClientMaxBodySize() << std::endl;
	// std::cout << "UPLOAD?: " << getUpload() << std::endl;
	// Request re;
	// addListen(re.getPort());
	// TODO quitar harcodeo

	addListen(7000);
	// addListen(3000);
	// addListen(3001);
	// addListen(3002);
	// addListen(8080);
	// addServerName(re.getHost());
	addServerName("localhost");
	addCgi(".py", "/usr/bin/python3");
	setRoot("www");
	addIndex("index.html");
	Config * location = new Config(this);
	// addLocation(re.getPath(), location);
	addLocation("/data", location);
	addErrorPage(404, "/404.html");
	setAutoindex(false);
}

Config::Config(Config *parent) : _parent (parent)
{
	setRoot("www/data");
	addIndex("index.html");
	addLimitExcept("GET");
	// addLimitExcept(re.getMethod());
}

Config::Config(std::string file) : _parent(NULL)
{
	std::cout << file << std::endl;
}

// Config::Config(const Config &other)
// {
// 	(void)other;
// 	// std::cout << "copy\n";
// }

Config::~Config()
{
	// std::cout << "destr\n";
}

Config * Config::getParent() const
{
	return _parent;
}

std::vector<int> Config::getListen() const
{
	return _listen;
}

std::vector<std::string> Config::getServerName() const
{
	return _serverName;
}

std::string Config::getRoot() const
{
	return _root;
}

std::map<std::string, std::string> Config::getCgi() const
{
	return _cgi;
}

std::vector<std::string> Config::getIndex() const
{
	return _index;
}

std::map<std::string, Config *> Config::getLocation() const
{
	return _location;
}

std::vector<std::string> Config::getLimitExcept() const
{
	return _limitExcept;
}

std::map<int, std::string> Config::getErrorPage() const
{
	return _errorPage;
}

bool Config::getAutoindex() const
{
	return _autoindex;
}

long Config::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

std::string Config::getUpload() const
{
	return _upload;
}

void Config::addListen(int listen)
{
	_listen.push_back(listen);
}

void Config::addServerName(std::string serverName)
{
	_serverName.insert(_serverName.end(), serverName);
}

void Config::addCgi(std::string key, std::string value)
{
	_cgi[key] = value;
}

void Config::setRoot(std::string root)
{
	_root = root;
}

void Config::addIndex(std::string index)
{
	_index.insert(_index.end(), index);
}

void Config::addLocation(std::string path, Config *location)
{
	// (void)path;
	// (void)location;
	_location[path] = location;
}

void Config::addLimitExcept(std::string method)
{
	_limitExcept.insert(_limitExcept.end(), method);
}

void Config::addErrorPage(int code, std::string path)
{
	_errorPage[code] = path;
}

void Config::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}

void Config::setClientMaxBodySize(float limit)
{
	_clientMaxBodySize = limit;
}

void Config::setUpload(std::string path)
{
	_upload = path;
}
