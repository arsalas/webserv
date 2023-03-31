#include <vector>
#include <map>
#include "Server.hpp"
#include "Utils/Strings.hpp"

Server::Server() {}
Server::Server(Config config) : _config(config)
{
	std::vector<int> listens = config.getListen();
}

Server::~Server()
{
}

std::string Server::getPathFolder(std::string path)
{
	std::vector<std::string> parts = Strings::split(Strings::trim(path, "/"), "/");

	std::map<std::string, Config *> location = _config.getLocation();
	std::map<std::string, Config *>::iterator it = location.begin();
	for (; it != location.end(); it++)
	{
		std::vector<std::string> partsServer = Strings::split(Strings::trim((*it).first, "/"), "/");

		bool isMatch = true;
		size_t i = 0;
		for (; i < partsServer.size(); i++)
		{
			if (partsServer[i] != parts[i])
			{
				isMatch = false;
				break;
			}
		}
		if (isMatch)
		{
			std::string finalPath = (*it).second->getRoot();
			for (; i < parts.size(); i++)
			{
				finalPath += "/" + parts[i];
			}
			return finalPath;
		}
	}
	return _config.getRoot() + path;
}

bool Server::isMethodAllow(std::string method)
{
	Config *conf = selectConfig(_activePath);
	std::vector<std::string> methods;
	if (conf)
		methods = conf->getLimitExcept();
	else
		methods = _config.getLimitExcept();
	if (methods.size() == 0)
		return true;
	for (size_t i = 0; i < methods.size(); i++)
	{
		if (method == methods[i])
			return true;
	}

	return false;
}

std::vector<std::string> Server::findIndex()
{
	Config *conf = selectConfig(_activePath);
	if (conf)
		return conf->getIndex();
	else
		return _config.getIndex();
}

bool Server::haveAutoindex()
{
	Config *conf = selectConfig(_activePath);
	if (conf)
		return conf->getAutoindex();
	else
		return _config.getAutoindex();
}

std::string Server::getErrorPageByStatus(int status)
{
	Config *conf = selectConfig(_activePath);
	try
	{
		if (conf)
			return conf->getErrorPage().at(status);
		else
			throw std::exception();
	}
	catch (const std::exception &e)
	{
		try
		{
			return _config.getErrorPage().at(status);
		}
		catch (const std::exception &e)
		{
			return "src/Templates/NotFound.html";
		}
	}
}

std::string Server::getRewrite()
{
	Config *conf = selectConfig(_activePath);
	if (conf)
		return conf->getRewrite();
	return _config.getRewrite();
}

int Server::getBodyMaxSize()
{
	Config *conf = selectConfig(_activePath);
	if (conf)
		return conf->getClientMaxBodySize();
	return _config.getClientMaxBodySize();
}

std::string Server::getUploadPath()
{
	Config *conf = selectConfig(_activePath);
	if (conf)
	{
		std::string root = conf->getRoot();
		if (root.empty())
			root = _config.getRoot();
		std::string upload = conf->getUpload();
		if (upload.empty())
			upload = _config.getUpload();
		return root + "/" + upload;
	}
	std::string root = _config.getRoot();
	std::string upload = _config.getUpload();
	return root + "/" + upload;
}

std::vector<int> Server::getSocketFd() const
{
	return _socketFd;
}

std::vector<int> Server::getPorts() const
{
	return _config.getListen();
}

void Server::setActivePath(std::string activePath)
{
	_activePath = activePath;
}

Config *Server::selectConfig(std::string path)
{
	std::vector<std::string> parts = Strings::split(Strings::trim(path, "/"), "/");

	std::map<std::string, Config *> location = _config.getLocation();
	std::map<std::string, Config *>::iterator it = location.begin();
	for (; it != location.end(); it++)
	{
		std::vector<std::string> partsServer = Strings::split(Strings::trim((*it).first, "/"), "/");

		bool isMatch = true;
		size_t i = 0;
		for (; i < partsServer.size(); i++)
		{
			if (partsServer[i] != parts[i])
			{
				isMatch = false;
				break;
			}
		}
		if (isMatch)
			return (*it).second;
	}
	return NULL;
}
