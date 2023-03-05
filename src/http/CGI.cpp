#include "CGI.hpp"

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers) : file_(file), config_(config), req__headers(req_headers) {
  _req_body = file_.getContent();
}

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body) : file_(file), config_(config), req__headers(req_headers) {
  if (req_body.empty() && config_.getMethod() != "POST")
    _req_body = file_.getContent();
  else
    _req_body = req_body;
}

void CGI::init(int worker_id) {
  char *cwd = getcwd(NULL, 0);
  if (!cwd)
    return ;
  cwd_ = cwd;
  free(cwd);

  env_ = NULL;
  argv_[0] = NULL;
  argv_[1] = NULL;
  argv_[2] = NULL;
  extension_ = file_.getMimeExtension();
  cgi_exe_ = config_.getCGI()[extension_];
  if (config_.getCGIBin()[0] == '/') {
    cgi__path = config_.getCGIBin() + "/" + cgi_exe_;
  } else {
    cgi__path = cwd_ + "/" + config_.getCGIBin() + "/" + cgi_exe_;
  }
  std::string cgi_path = "/tmp/webserv_cgi_tmp_" + ft::to_string(worker_id);
  tmp_file_.set_path(cgi_path.c_str());
  tmp_file_.open(true);
  if (worker_id)
    Log.print(DEBUG, "worker[" + ft::to_string(worker_id) + "] : CGI -> " + cgi__path);
  else
    Log.print(DEBUG, "server : CGI -> " + cgi__path);
}

CGI::~CGI() {
  free(argv_[0]);
  free(argv_[1]);
  if (env_)
    ft::free_tab(env_);
  tmp_file_.close();
  tmp_file_.unlink();
}

int CGI::execute() {
  file__path = cwd_ + "/" + file_.getPath();

  if (!setCGIEnv())
    return 500;
  if (!(argv_[0] = ft::strdup(cgi__path.c_str())))
    return 500;
  if (!(argv_[1] = ft::strdup(file__path.c_str())))
    return 500;
  argv_[2] = NULL;

  int pip[2];

  if (pipe(pip) != 0)
    return 500;

  pid_t pid = fork();

  if (pid == 0) {
    if (chdir(file__path.substr(0, file__path.find_last_of('/')).c_str()) == -1)
      return 500;
    close(pip[1]);
    if (dup2(pip[0], 0) == -1)
      return 500;
    if (dup2(tmp_file_.getFd(), 1) == -1)
      return 500;
    close(pip[0]);
    execve(argv_[0], argv_, env_);
    exit(1);
  }
  else if (pid > 0) {
    close(pip[0]);
    if (_req_body.length() && write(pip[1], _req_body.c_str(), _req_body.length()) <= 0)
      return 500;
    close(pip[1]);

    int status;

    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
  }
  else {
    return 502;
  }

  body_ = tmp_file_.getContent();
  return 200;
}

void CGI::parseHeaders(std::map<std::string, std::string> &headers) {
  size_t end, last;
  std::string header;

  while ((end = body_.find("\r\n")) != std::string::npos) {
    if (body_.find("\r\n") == 0) {
      body_.erase(0, end + 2);
      break;
    }
    if ((last = body_.find(':', 0)) != std::string::npos) {
      header = body_.substr(0, last);
      headers[header] = ft::trim_left(body_.substr(last + 1, end - last - 1), ' ');
    }
    body_.erase(0, end + 2);
  }
  if (headers.count("Content-Length")) {
    size_t size = ft::stoi(headers["Content-Length"]);

    body_.erase(size);
  }
}

std::string &CGI::getBody() {
  return body_;
}

bool CGI::setCGIEnv() {
  if (config_.getMethod() == "POST") {
		cgi_env_["CONTENT_TYPE"] = req__headers["Content-Type"];
		cgi_env_["CONTENT_LENGTH"] = ft::to_string(_req_body.length());
	}
	cgi_env_["GATEWAY_INTERFACE"] = "CGI/1.1";
  cgi_env_["_pathINFO"] = file__path;
	cgi_env_["_pathTRANSLATED"] = file__path;
  cgi_env_["QUERY_STRING"] = config_.getQuery();
  cgi_env_["REMOTE_ADDR"] = config_.getClient().getAddr();

  if (config_.getAuth() != "off") {
    cgi_env_["AUTH_TYPE"] = "Basic";
    cgi_env_["REMOTE_IDENT"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
    cgi_env_["REMOTE_USER"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
  }

  cgi_env_["_requestMETHOD"] = config_.getMethod();
	cgi_env_["_requestURI"] = file__path;

  cgi_env_["SCRIPT_NAME"] = cgi__path;
	cgi_env_["SERVER_NAME"] = config_.getHost();
	cgi_env_["SERVER_PROTOCOL"] = config_.getProtocol();
	cgi_env_["SERVER_PORT"] = ft::to_string(config_.getPort());
  cgi_env_["SERVER_SOFTWARE"] = "WEBSERV/1.0";

	if (extension_ == ".php")
		cgi_env_["REDIRECT_STATUS"] = "200";

  for (std::map<std::string, std::string, ft::comp>::iterator it = req__headers.begin(); it != req__headers.end(); it++) {
    if (!it->second.empty()) {
      std::string header = "HTTP_" + ft::to_upper(it->first);
      std::replace(header.begin(), header.end(), '-', '_');
      cgi_env_[header] = it->second;
    }
  }

	if (!(env_ = (char **)malloc(sizeof(char *) * (cgi_env_.size() + 1))))
		return false;

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = cgi_env_.begin(); it != cgi_env_.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		if (!(env_[i] = ft::strdup(tmp.c_str())))
      return false;
		i++;
	}
	env_[i] = NULL;
  return true;
}