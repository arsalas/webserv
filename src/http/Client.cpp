#include "Client.hpp"

Client::Client(int fd, std::string &addr, Listen &host_port, int worker_id, bool disconnect) : fd_(fd), addr_(addr), host_port_(host_port), worker_id_(worker_id), disconnect_(disconnect) {
  _request = NULL;
  config_ = NULL;
  response_ = NULL;
}

Client::~Client() {
  close(fd_);
  clear();
}

void Client::clear() {
  ft::delete_(_request);
  ft::delete_(config_);
  ft::delete_(response_);
}

void Client::setupConfig(std::vector<ServerConfig> &servers, InputArgs &options) {
  config_ = new RequestConfig(*_request, host_port_, servers, *this);
  config_->setup(options);
}

void Client::setupResponse(std::vector<ServerConfig> &servers, InputArgs &options, int error_code) {
  if (!_request)
    _request = getRequest(true);

  if (!config_)
    setupConfig(servers, options);

  response_ = new Response(*config_, worker_id_, error_code);

  int loop = 0;

  for (int ret = 1; ret != 0; loop++) {
    ret = 0;

    response_->build();

    if (response_->redirect()) {
      ret = 1;
      config_->redirectLocation(response_->redirect_target());
      Log.print(INFO, "INTERNAL REDIRECT : [target: " + response_->redirect_target() + "]");
      response_->clear();
    }
    if (loop >= 10) {
      ft::delete_(response_);
      response_ = new Response(*config_, worker_id_, 500);
      response_->build();
      break ;
    }
  }

  ft::delete_(_request);
}

bool Client::timeout() {
  if (_request) {
    time_t current_time = ft::get_current_time_in_sec();
    if (current_time - _request->get_start_timer_in_sec() > START_TIMEOUT
      || current_time - _request->get_last_timer_in_sec() > LAST_TIMEOUT) {
      if (_request->timeout())
        return true;
    }
  }
  return false;
}

bool Client::disconnect() {
  if (disconnect_)
    return true;
  return false;
}

int Client::getFd() {
  return fd_;
}

std::string &Client::getAddr() {
  return addr_;
}

Request *Client::getRequest(bool force) {
  if (!_request && force)
    _request = new Request();
  return _request;
}

RequestConfig *Client::getConfig() {
  return config_;
}

Response *Client::getResponse() {
  return response_;
}