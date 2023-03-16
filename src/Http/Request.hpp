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
    int                                 _port;
    std::map<std::string, std::string>  _header;
    std::map<std::string, std::string>  _formData;

    private:
    int     tokenRequest(std::string req);
    int     errorsToken();

    Request();

    public:
    Request(std::string req);

    /*      SETTERS     */
    void    setMethod(std::vector<std::string> lineVector);
    void    setHttp(std::vector<std::string> lineVector);
    void    setPath(std::vector<std::string> lineVector);
    void    setHeader(std::vector<std::string> lineVector);
    void    setFormData(std::vector<std::string> lineVector);
    void    setBody(std::vector<std::string> lineVector);
    void    setHostPort(std::vector<std::string> lineVector);

    /*      GETTERS     */
    std::string getMethod(void) const;
    std::string getHttp(void) const;
    std::string getPath(void) const;
    std::map<std::string, std::string>  getHeader(void) const;
    std::string getBody(void) const;
    int         getPort(void) const;
    std::string getHost(void) const;
};
