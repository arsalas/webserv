#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <fcntl.h>

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"

class Config
{
private:
	int							fd_;
	int							_workers;
	std::string					_path;
	std::string					_fileContent;
	std::ifstream				fdStream_;
	std::vector<std::string>	_tokens;
	std::vector<ServerConfig>	_servers;

public:
	Config(std::string &path);
	~Config();

	void	tokenize();
	void	openBrackets(std::stack<bool> &brackets, std::string &tmp);
	void	closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx);
	void	endOfLine(std::string &tmp);
	void	pushToken(std::string &tmp);

	void	parse();
	void	serverToken(int i, std::vector<std::string>::iterator &it);
	void	workersToken(std::vector<std::string>::iterator &it);

	int							getWorkers();
	std::vector<ServerConfig>	&getServers();
	std::string					&getPath();
	std::string					&getFileContent();

	void		clear();
	std::string	trim(const std::string &s);
	std::string rtrim(const std::string &s);
	std::string ltrim(const std::string &s);
};

#endif
