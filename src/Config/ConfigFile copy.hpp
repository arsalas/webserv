#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <fcntl.h>

#include "Server/Server.hpp"
#include "ServerConfig.hpp"
#include "Utils/Strings.hpp"

class ConfigFile
{
private:
    int _fd;
    int _workers; // TODO Borrar
    std::string _path;
    std::string _fileContent;
    std::ifstream fdStream_;
    std::vector<std::string> _tokens;
    std::vector<Server> _servers;

public:
    ConfigFile(std::string &path);
    ~ConfigFile();

    void tokenize();
    void openBrackets(std::stack<bool> &brackets, std::string &tmp);
    void closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx);
    void endOfLine(std::string &tmp);
    void pushToken(std::string &tmp);

    int getWorkers();
    std::vector<Server> &getServers();
    std::string &getPath();
    std::string &getFileContent();

    void clear();

private:
    void parse();
    void serverToken(int i, std::vector<std::string>::iterator &it);
    void workersToken(std::vector<std::string>::iterator &it);
    bool isValidDirective(std::string const &str);

public:
    class NotOpenFile : public std::exception
    {
        virtual const char *what() const throw();
    };

    class config_exception : virtual public std::exception
    {
    public:
        config_exception(std::string msg, int err_num = 0, std::string arg = "") : _errno(err_num)
        {
            _errorMsg = msg;
            if (_errorMsg.find('%') != std::string::npos)
                _errorMsg.replace(_errorMsg.find('%'), 1, arg);
            if (_errno)
            {
                _errorMsg = _errorMsg + " (" + Strings::intToString(_errno) + ": " + strerror(_errno) + ")";
            }
        }
        virtual ~config_exception() throw() {}

        const char *what() const throw()
        {
            return (_errorMsg.c_str());
        }

    private:
        int _errno;
        std::string _errorMsg;
    };
};
