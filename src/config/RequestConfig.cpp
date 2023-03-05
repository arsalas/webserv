#include "RequestConfig.hpp"

/*
** Constructors & Deconstructors
*/

RequestConfig::RequestConfig(Request &request, Listen &host_port, std::vector<ServerConfig> &servers, Client &client) :
  request_(request),
  host_port_(host_port),
  _servers(servers),
  client_(client) {}

RequestConfig::~RequestConfig() {
}

void RequestConfig::setup(InputArgs &options) {
  ServerConfig *server = getServerForRequest(_servers);
  ServerConfig *location = NULL;

  if (request_.getStatus() > 2)
    location = getLocationForRequest(server, request_.target_);

  server_ = server;
  location_ = server;

  target_ = request_.target_;

  if (location) {
    location_ = location;
    if (!options.location() && request_.target_.find(location->_uri) != std::string::npos)
      target_.erase(0, location_->_uri.length());
  }
}

void RequestConfig::redirectLocation(std::string target) {
  ServerConfig *location = NULL;

  if (request_.getStatus() > 2)
    location = getLocationForRequest(server_, target);

  target_ = target;
  if (location) {
    location_ = location;
    if (target_.find(location->_uri) != std::string::npos)
      target_.erase(0, location_->_uri.length());
  }
}

ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::vector<ServerConfig*> matching_servers;

  // Match server based on request ip + port
  for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++) {
    for (std::vector<Listen>::iterator list = it->_listens.begin(); list != it->_listens.end(); list++) {
      if ((list->ip_ == host_port_.ip_ || list->ip_ == "0.0.0.0") && list->port_ == host_port_.port_) {
        matching_servers.push_back(&(*it));
        break;
      }
    }
  }

  // If only one match, it's our server
  if (matching_servers.size() == 1)
    return matching_servers.front();

  std::string host = request_.headers_["Host"].substr(0, request_.headers_["Host"].find(':'));

  // We match based on server names
  for (std::vector<ServerConfig*>::iterator it = matching_servers.begin(); it != matching_servers.end(); it++) {
    std::vector<std::string> server_names = (*it)->getServerNames();

    for (std::vector<std::string>::iterator server_name = server_names.begin(); server_name != server_names.end(); server_name++) {
      if (host == *server_name)
        return *it;
    }
  }

  // We use default server (first one)
  return matching_servers.front();
}

ServerConfig *RequestConfig::match_regexp(std::vector<ServerConfig*> &locations, std::string &target) {
  regex_t reg;

  for (std::vector<ServerConfig*>::iterator it = locations.begin(); it != locations.end(); it++) {
    int flag = REG_NOSUB | REG_EXTENDED;

    if ((*it)->_modifier == 3)
      flag |= REG_ICASE;

    int err = regcomp(&reg, (*it)->_uri.c_str(), flag);

    if (err == 0) {
      int match = regexec(&reg, target.c_str(), 0, NULL, 0);
      regfree(&reg);
      if (match == 0)
        return *it;
    }
  }
  return NULL;
}

ServerConfig *RequestConfig::getLocationForRequest(ServerConfig *server, std::string &target) {
  ServerConfig *location = NULL;

  std::vector<ServerConfig*> reg_locations;

  for (std::vector<ServerConfig>::iterator it = server->_locations.begin(); it != server->_locations.end(); it++) {
    if (it->_modifier != 2 && it->_modifier != 3) {
      if (it->_modifier == 1 && it->_uri == target)
        return &(*it);
      else if (target.find(it->_uri) == 0) {
        if (location && location->_uri.length() < it->_uri.length())
          location = &(*it);
        else if (!location)
          location = &(*it);
      }
    }
    else
      reg_locations.push_back(&(*it));
  }

  if (location && location->_modifier == 4)
    return location;

  if (location && !location->_locations.empty()) {
    for (std::vector<ServerConfig>::iterator it = location->_locations.begin(); it != location->_locations.end(); it++) {
      if (it->_modifier == 2 || it->_modifier == 3) {
        reg_locations.insert(reg_locations.begin(), &(*it));
      }
    }
  }

  ServerConfig *reg = match_regexp(reg_locations, target);

  if (reg)
    return reg;

  return location;
}

bool RequestConfig::methodAccepted(std::string &method) {
  std::vector<std::string> methods = location_->methods_;

  if (methods.empty())
    return true;
  if (std::find(methods.begin(), methods.end(), method) != methods.end())
    return true;
  return false;
}

/* GETTERS ! */

std::string &RequestConfig::getTarget() {
  return target_;
}

std::string &RequestConfig::getRequestTarget() {
  return request_.target_;
}

std::string &RequestConfig::getQuery() {
  return request_.query_string_;
}

void RequestConfig::setTarget(std::string target) {
  target_ = target;
}

std::string &RequestConfig::getHost() {
  return host_port_.ip_;
}

uint32_t &RequestConfig::getPort() {
  return host_port_.port_;
}

Client &RequestConfig::getClient() {
  return client_;
}

std::string &RequestConfig::getRoot() {
  return location_->_root;
}

std::string &RequestConfig::getAuth() {
  return location_->_credentials;
}

std::string &RequestConfig::getUri() {
  return location_->_uri;
}

std::map<std::string, std::string> &RequestConfig::getCGI() {
  return location_->cgi_;
}

std::string &RequestConfig::getCGIBin() {
  return location_->_cgiBin;
}

size_t &RequestConfig::getClientMaxBodySize() {
  return location_->client_max_body_size_;
}

bool RequestConfig::getAutoindex() {
  return location_->_autoindex;
}

std::string &RequestConfig::getUpload() {
  return location_->upload_;
}

std::vector<std::string> &RequestConfig::getIndexes() {
  return location_->_index;
}

std::map<int, std::string> &RequestConfig::getErrorPages() {
  return location_->_error_codes;
}

std::vector<std::string> &RequestConfig::getMethods() {
  return location_->methods_;
}

std::string &RequestConfig::getBody() {
  return request_.req_body_;
}

std::string &RequestConfig::getMethod() {
  return request_.method_;
}

std::string &RequestConfig::getHeader(std::string key) {
  return request_.headers_[key];
}

std::map<std::string, std::string, ft::comp> &RequestConfig::getHeaders() {
  return request_.headers_;
}

std::string &RequestConfig::getProtocol() {
  return request_.protocol_;
}

std::string RequestConfig::log(LogLevel level) {
  std::string ret;

  ret = ret + "[method: " + getMethod() + "]";
  ret = ret + " [target: " + getRequestTarget() + "]";
  ret = ret + " [server: " + ft::to_string(server_->id_) + "]";
  ret = ret + " [location: " + getUri() + "]";
  if (level > INFO) {
    for (std::map<std::string, std::string>::iterator it = getHeaders().begin(); it != getHeaders().end(); it++)
      ret = ret + "\n" + it->first + ": " + it->second;
  }
  return ret;
}
