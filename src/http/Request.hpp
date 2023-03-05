#ifndef _requestHPP
# define _requestHPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# include "Client.hpp"
# include "Config.hpp"
// # include "RequestConfig.hpp"
# include "Response.hpp"
# include "StringUtils.hpp"

class Request {
 public:
  friend class RequestConfig;

  Request();
  ~Request();

  int parse(std::string &buffer);
  int parse(char buffer[], int nbytes);
  int _methodline();
  int headers();
  int prebody();
  int body();
  int chunk();
  int chunk_trailer();

  bool timeout();
  time_t get_start_timer_in_sec();
  time_t get_last_timer_in_sec();

  int getStatus();

  enum Status {
    FIRST_LINE,
    HEADERS,
    PREBODY,
    BODY,
    CHUNK,
    COMPLETE,
    ERROR
  };

  enum ChunkStatus {
    CHUNK_BODY,
    CHUNK_SIZE,
  };

 private:
  std::string buffer_;

  std::string _method;
  std::string _target;
  std::string query_string_;
  std::string _protocol;
  std::string _req_body;
  std::map<std::string, std::string, ft::comp> _headers;

  int body_offset_;
  int chunk_size_;
  struct timeval start_timer_;
  struct timeval last_timer_;
  size_t length_;

  Status status_;
  ChunkStatus chunk_status_;
};

#endif
