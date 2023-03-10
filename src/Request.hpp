#include <iostream>
#include <map>
#include <vector>

class Request
{
    private:
    std::string _method;
    std::string _path;
    std::map<std::string, std::string> _headers;
    std::string _body;

    public:
    void tokenRequest(void);
    void    insertMethod(std::vector<std::string> lineVector);
    void    insertPath(std::vector<std::string> lineVector);
    void    insertHeader(std::vector<std::string> lineVector);
};


