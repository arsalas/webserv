#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>

#include <sys/types.h>
#include <sys/wait.h>

#include "RequestConfig.hpp"
#include "File.hpp"
#include "StringUtils.hpp"

class RequestConfig;

class CGI
{

private:
  File &_file;
  RequestConfig &config_;
  std::map<std::string, std::string, ft::comp> &req__headers;
  std::string cgi__path;
  std::string cgi_exe_;
  std::string extension_;
  std::string cwd_;
  std::string file__path;
  std::string body_;
  std::string _req_body;
  File tmp_file_;
  std::map<std::string, std::string> cgi_env_;
  char **env_;
  char *argv_[3];

public:
  CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers);
  CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body);
  ~CGI();

  void init(int worker_id);
  int execute();
  bool setCGIEnv();
  void parseHeaders(std::map<std::string, std::string> &headers);
  std::string &getBody();
};

#endif