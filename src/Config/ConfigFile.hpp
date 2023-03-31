#pragma once

#include <unistd.h>
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
    std::vector<Server> _server;

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
    void serverToken(int i, std::vector<std::string>::iterator &it);

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

};
