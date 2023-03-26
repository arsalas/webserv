#include <iostream>
#include <map>
#include <vector>
#include <fstream>

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
    std::vector<std::string>            _ContentDisposition;
    std::string                         _filename;
    std::string                         _extension;
    std::vector<std::string>            _fileContent;

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
    void    setContentDisposition(std::vector<std::string> lineVector);
    void    setFilename( void );
    void    createFilename( void );
    void    setFileContent(std::vector<std::string> lineVector);

    /*      GETTERS     */
    std::string getMethod(void) const;
    std::string getHttp(void) const;
    std::string getPath(void) const;
    std::map<std::string, std::string>  getHeader(void) const;
    std::string getBody(void) const;
    int         getPort(void) const;
    std::string getHost(void) const;
};
