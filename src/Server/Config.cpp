#include "Config.hpp"

Config::Config()
{
	// TODO quitar harcodeo
	addListen(3000);
	addServerName("localhost");
	addCgi(".py", "/usr/bin/python3");
	setRoot("www");
	addIndex("index.html");
	Config location;
	location.setRoot("www/data");
	location.addIndex("index.html");
	location.addLimitExcept("GET");
	addLocation("/data", location);
	addErrorPage(404, "/404.html");
	setAutoindex(false);
}

Config::Config(std::string file)
{
	std::cout << file << std::endl;
}

Config::~Config() {}

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

std::map<std::string, Config> Config::getLocation() const
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

std::string Config::getUploads() const
{
	return _uploads;
}

void Config::addListen(int listen)
{
	_listen.insert(_listen.end(), listen);
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

void Config::addLocation(std::string path, Config location)
{
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

void Config::setUploads(std::string path)
{
	_uploads = path;
}
