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

public:
    ConfigFile(std::string path);
    ~ConfigFile();

private:
    void token();
    void parse();
    void openBrackets(std::stack<bool> &brackets, std::string &tmp);
    void closeBrackets(std::stack<bool> &brackets, std::string &tmp);
    bool isValidDirective(std::string const &str);
    void endOfLine(std::string &tmp);
    void pushToken(std::string &tmp);
    void configToken(std::vector<std::string>::iterator &it);
    void checkValidDir(std::vector<std::string>::iterator &it);

    void listen(std::vector<std::string>::iterator &it);
    void savePort(std::string &str, std::string &ip, uint32_t &port);

public:
    class EmptyFile : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class ExtraClosing : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class MissingDot : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class InvalidBlock : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class EmptyServer : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class InvalidFile : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class InvalidValue : public std::exception
    {
        virtual const char *what() const throw();
    };
};
