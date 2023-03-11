#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdint.h>

#include <StringUtils.hpp>

struct Listen
{
	std::string _ip;
	uint32_t port_;

	Listen() : _ip(""), port_(0){};
	Listen(std::string ip, uint32_t port) : _ip(ip), port_(port){};
};

inline bool operator==(const Listen &lhs, const Listen &rhs)
{
	return lhs._ip == rhs._ip && lhs.port_ == rhs.port_;
}

class ServerConfig
{
public:
	friend class RequestConfig;

	ServerConfig();
	~ServerConfig();

	ServerConfig	&operator=(const ServerConfig &copy);

	typedef void	(ServerConfig::*type)(std::vector<std::string>::iterator &);
	static std::map<std::string, type> _directive;
	static	void initDirectiveMap();

	void	server(std::vector<std::string>::iterator &it);

	void	listen(std::vector<std::string>::iterator &it);
	void	savePort(std::string &str, std::string &ip, uint32_t &port);
	void	saveListen(std::vector<std::string>::iterator &it, std::string &ip, uint32_t &port);

	void	serverName(std::vector<std::string>::iterator &it);

	void	root(std::vector<std::string>::iterator &it);

	void	cgi(std::vector<std::string>::iterator &it);

	void	index(std::vector<std::string>::iterator &it);
	void	cgi_bin(std::vector<std::string>::iterator &it);

	void	location(std::vector<std::string>::iterator &it);
	void	locationLoop(std::vector<std::string>::iterator &it, std::vector<ServerConfig> &locations);
	bool	isLocationModifier(std::string &str);
	void	modificateLocation(std::string &str);
	void	checkValidDir(std::vector<std::string>::iterator &it);

	void	limitExcept(std::vector<std::string>::iterator &it);

	void	upload(std::vector<std::string>::iterator &it);

	void	client_max_body_size(std::vector<std::string>::iterator &it);

	void 	auth(std::vector<std::string>::iterator &it);

	void	errorPage(std::vector<std::string>::iterator &it);

	void	autoindex(std::vector<std::string>::iterator &it);

	std::string					&getUri();
	std::vector<Listen>			&getListens();
	std::vector<std::string>	&getServerNames();
	std::vector<ServerConfig>	&getLocations();

	enum LocationModifier
	{
		NONE,
		EXACT,
		CASE_SENSITIVE_REG,
		CASE_INSENSITIVE_REG,
		LONGEST,
	};

	int id_;

private:
	std::vector<Listen>			_listens;
	std::vector<std::string>	server_name_;
	std::vector<ServerConfig>	_locations;
	std::string					_uri;
	LocationModifier			_modifier;
	std::string					_credentials;
	bool						_autoindex;
	size_t						_client_max_body_size;
	std::string					_root;
	std::string					_upload;
	std::vector<std::string>	_methods;
	std::map<int, std::string>	_error_codes;
	std::vector<std::string>	_index;
	std::map<std::string, std::string>	cgi_;
	std::string					_cgiBin;
};

#endif
