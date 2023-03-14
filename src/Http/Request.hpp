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
    int     tokenRequest(void);
    int     errorsToken();


    /*      SETTERS     */
    void    setMethod(std::vector<std::string> lineVector);
    void    setHttp(std::vector<std::string> lineVector);
    void    setPath(std::vector<std::string> lineVector);
    void    setHeader(std::vector<std::string> lineVector);
    void    setFormData(std::vector<std::string> lineVector);
    void    setBody(std::vector<std::string> lineVector);
    void    setHostPort(std::vector<std::string> lineVector);


    /*      GETTERS     */
    std::string getMethod(void);
    std::string getHttp(void);
    std::string getPath(void);
    std::map<std::string, std::string>  getHeader(void);
    std::string getBody(void);
    int         getPort(void);
    std::string getHost(void);

    public:
    Request();
};
