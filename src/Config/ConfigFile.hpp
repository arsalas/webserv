#pragma once

#include <algorithm>
#include <cctype>
#include <vector>
#include <locale>
#include <iostream>
#include <fstream>
#include <string>
#include "Utils/File.hpp"
#include "Server/Server.hpp"

class ConfigFile
{
private:
    std::string _file;
    std::string _fileContent;
    std::vector<std::string> _token;
    std::vector<std::string> _configToken;
    std::vector<Config> _configs;
    std::vector<Config> _locations;

public:
    ConfigFile(std::string path);
    ~ConfigFile();
    std::vector<Config> getConfigs();

private:
    void token();
    void parse();
    void openBrackets(std::stack<bool> &brackets, std::string &tmp);
    void closeBrackets(std::stack<bool> &brackets, std::string &tmp);
    bool isValidDirective(std::string const &str);
    void endOfLine(std::string &tmp);
    void pushToken(std::string &tmp);
    void configToken(std::vector<std::string>::iterator &it);
    void checkValidDir(std::vector<std::string>::iterator &it, Config conf);

    void listen(std::vector<std::string>::iterator &it, Config conf);
    void savePort(std::string &str, std::string &ip, uint32_t &port);
    void servername(std::vector<std::string>::iterator &it, Config &conf);
    void root(std::vector<std::string>::iterator &it, Config &conf);
    void cgi(std::vector<std::string>::iterator &it, Config &conf);
    void index(std::vector<std::string>::iterator &it,  Config &conf);
    void location(std::vector<std::string>::iterator &it);
    void locationLoop(std::vector<std::string>::iterator &it);
    void limitExcept(std::vector<std::string>::iterator &it, Config conf);
    void errorPage(std::vector<std::string>::iterator &it, Config conf);
    void client_max_body_size(std::vector<std::string>::iterator &it, Config conf);
    void autoindex(std::vector<std::string>::iterator &it, Config conf);
};
