#pragma once

#include <iostream>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <set>
#include "Config.hpp"

class Server
{
private:
	Config _config;
	std::vector<sockaddr_in> _servAddr;
	std::vector<int> _socketFd;
	std::string _activePath;

	// std::string _ip;
	// int _port;
	// int _socketFd;

public:
	Server();
	Server(Config);
	// Server(std::string, int);
	~Server();

	std::string getPathFolder(std::string path);
	bool isMethodAllow(std::string);
	std::vector<std::string> findIndex();
	bool haveAutoindex();
	std::string getErrorPageByStatus(int status);
	std::string getRewrite();
	int getBodyMaxSize();
	std::string getUploadPath();

	// GETTERS
	std::vector<int> getSocketFd() const;
	std::vector<int> getPorts() const;

	// SETTERS
	void setActivePath(std::string activePath);

private:
	Config *selectConfig(std::string path);
	// EXCEPTIONS
public:
};