#pragma once
#include <iostream>
#include <vector>
#include <map>

class Config
{
private:
	Config *_parent;
	std::vector<int> _listen;
	std::vector<std::string> _serverName;
	std::string _root;
	std::map<std::string, std::string> _cgi;
	std::vector<std::string> _index;
	std::map<std::string, Config *> _location;
	std::vector<std::string> _limitExcept;
	std::map<int, std::string> _errorPage;
	bool _autoindex;
	long _clientMaxBodySize;
	std::string _upload;

public:
	// TODO pasar text config para iniciar la clase, quitar el bool
	Config();
	Config(Config *parent);
	Config(std::string file);
	// Config(const Config &other);

	~Config();

	std::vector<int> getListen() const;

public:
	// GETTERS
	Config *getParent() const;
	std::vector<std::string> getServerName() const;
	std::string getRoot() const;
	std::map<std::string, std::string> getCgi() const;
	std::vector<std::string> getIndex() const;
	std::map<std::string, Config *> getLocation() const;
	std::vector<std::string> getLimitExcept() const;
	std::map<int, std::string> getErrorPage() const;
	bool getAutoindex() const;
	long getClientMaxBodySize() const;
	std::string getUpload() const;

public:
	//  SETTERS
	void addListen(int listen);
	void addServerName(std::string serverName);
	void addCgi(std::string key, std::string value);
	void setRoot(std::string root);
	void addIndex(std::string index);
	void addLocation(std::string, Config *location);
	void addLimitExcept(std::string method);
	void addErrorPage(int code, std::string path);
	void setAutoindex(bool autoindex);
	void setClientMaxBodySize(float limit);
	void setUpload(std::string path);
};
