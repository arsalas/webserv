#include <iostream>
#include <map>
#include <vector>

class Request
{
    private:
    std::string                         _method;
    std::string                         _path;
    std::string                         _body;
    std::map<std::string, std::string>  _header;

    public:
    void    tokenRequest(void);
    void    insertMethod(std::vector<std::string> lineVector);
    void    insertPath(std::vector<std::string> lineVector);
    void    insertHeader(std::vector<std::string> lineVector);
    void    insertBody(std::vector<std::string> lineVector);

    /*      GETTERS     */
    std::string getMethod(void);
    std::string getPath(void);
    std::map<std::string, std::string>  getHeader(void);
    std::string getBody(void);
};
