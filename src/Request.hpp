#include <iostream>
#include <map>
#include <vector>

class Request
{
    private:
    std::string                         _method;
    std::string                         _http;
    std::string                         _path;
    std::string                         _body;
    std::map<std::string, std::string>  _header;

    public:
    int     tokenRequest(void); 
    int     errorsToken();
    void    setMethod(std::vector<std::string> lineVector);
    void    setHttp(std::vector<std::string> lineVector);
    void    setPath(std::vector<std::string> lineVector);
    void    setHeader(std::vector<std::string> lineVector);
    void    setBody(std::vector<std::string> lineVector);

    /*      GETTERS     */
    std::string getMethod(void);
    std::string getHttp(void);
    std::string getPath(void);
    std::map<std::string, std::string>  getHeader(void);
    std::string getBody(void);
};
