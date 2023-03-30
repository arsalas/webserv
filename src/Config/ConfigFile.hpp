#pragma once
#include <iostream>
#include "Utils/File.hpp"

class ConfigFile
{
public:
    ConfigFile(std::string path);
    ~ConfigFile();


private:
    std::string _file;
    std::string _fileContent;
    std::vector<std::string> _token;
    std::vector<Server> _server;

private:
    void token();
    void parse();
    void openBrackets(std::stack<bool> &brackets, std::string &tmp);
    void closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx);
    bool isValidDirective(std::string const &str);
    void endOfLine(std::string &tmp);
    void pushToken(std::string &tmp);

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
