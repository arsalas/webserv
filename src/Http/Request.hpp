#include <iostream>
#include <map>
#include <vector>
#include <fstream>

class Request
{
    protected:
	// _headers;
	// _payload;
	// _files;
	// _path;
	// _method;


    std::string                         _method;
    std::string                         _http;
    std::string                         _path;
    std::string                         _body;
    std::string                         _host;
    int                                 _port;
    std::map<std::string, std::string>  _header;
    std::string                         _boundary;
    std::string                         _filename;
    std::vector<std::string>            _fileContent;
    std::map<std::string, std::string>  _mapFiles;
    std::map<std::string, std::string>  _mapPayload;

    std::string _extension;
    std::map<std::string, std::string> _formData;
    std::vector<std::string> _ContentDisposition;

private:
    Request();
    int tokenRequest(std::string req);
    int errorsToken();

public:
    Request(std::string req);

    /*      SETTERS     */
    void setMethod(std::vector<std::string> lineVector);
    void setHttp(std::vector<std::string> lineVector);
    void setPath(std::vector<std::string> lineVector);
    void setHeader(std::vector<std::string> lineVector);
    void setHostPort(std::vector<std::string> lineVector);

    bool isContentType(void);
    void setBoundary(void);
    void setFile(std::vector<std::string> lineVector);
    void setFileName(std::string str);
    void createFilename(void);
    void setFileContent(std::vector<std::string> lineVector);

    void setPayload(std::vector<std::string> lineVector);
    void setMapFiles(std::vector<std::string> lineVector);

    /*      GETTERS     */
    std::string getMethod(void) const;
    std::string getHttp(void) const;
    std::string getPath(void) const;
    std::string getHost(void) const;
    int getPort(void) const;
    std::map<std::string, std::string> getHeader(void) const;
    std::string getBoundary(void) const;
    std::string getFilename(void) const;
    std::map<std::string, std::string> getMapFiles(void) const;
    std::map<std::string, std::string> getMapPayload(void) const;
    // std::string getBody(void) const;

public:
    class InvalidMethod : public std::exception
    {
        virtual const char *what() const throw();
    };

public:
    class InvalidProtocol : public std::exception
    {
        virtual const char *what() const throw();
    };
};
