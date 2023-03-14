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
    std::string                         _host;
    std::string                         _port;
    std::map<std::string, std::string>  _header;
    std::map<std::string, std::string>  _formData;

    private:
    int     errorsToken();
    void    setMethod(std::vector<std::string> lineVector);
    void    setHttp(std::vector<std::string> lineVector);
    void    setPath(std::vector<std::string> lineVector);
    void    setHeader(std::vector<std::string> lineVector);
    void    setFormData(std::vector<std::string>::iterator itVector, std::vector<std::string> auxVector, std::vector<std::string>::iterator endVector);
    void    setBody(std::vector<std::string> lineVector);
    void    setHostPort(std::vector<std::string> lineVector);

    std::map<std::string, std::string> mapSplit(std::vector<std::string> auxVector, std::string delimiter);


    /*      GETTERS     */
    std::string getMethod(void);
    std::string getHttp(void);
    std::string getPath(void);
    std::map<std::string, std::string>  getHeader(void);
    std::string getBody(void);
    std::string         getPort(void);
    std::string getHost(void);

    public:
    int     tokenRequest(void);
};
