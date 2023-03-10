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
};


