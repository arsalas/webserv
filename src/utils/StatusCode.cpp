# include "StatusCode.hpp"

StatusCode::StatusCode() {
  // Informational 1xx
  _error_codes[100] = "Continue";
  _error_codes[101] = "Switching Protocols";

  // Successful 2xx
  _error_codes[200] = "OK";
  _error_codes[201] = "Created";
  _error_codes[202] = "Accepted";
  _error_codes[203] = "Non-Authoritative Information";
  _error_codes[204] = "No Content";
  _error_codes[205] = "Reset Content";
  _error_codes[206] = "Partial Content";

  // Redirection 3xx
  _error_codes[300] = "Multiple Choices";
  _error_codes[301] = "Moved Permanently";
  _error_codes[302] = "Found";
  _error_codes[303] = "See Other";
  _error_codes[304] = "Not Modified";
  _error_codes[305] = "Use Proxy";
  _error_codes[307] = "Temporary Redirect";

  // Client Error 4xx
  _error_codes[400] = "Bad Request";
  _error_codes[401] = "Unauthorized";
  _error_codes[402] = "Payment Required";
  _error_codes[403] = "Forbidden";
  _error_codes[404] = "Not Found";
  _error_codes[405] = "Method Not Allowed";
  _error_codes[406] = "Not Acceptable";
  _error_codes[407] = "Proxy Authentication Required";
  _error_codes[408] = "Request Timeout";
  _error_codes[409] = "Conflict";
  _error_codes[410] = "Gone";
  _error_codes[411] = "Length Required";
  _error_codes[412] = "Precondition Failed";
  _error_codes[413] = "Payload Too Large";
  _error_codes[414] = "URI Too Long";
  _error_codes[415] = "Unsupported Media Type";
  _error_codes[416] = "Range Not Satisfiable";
  _error_codes[417] = "Expectation Failed";
  _error_codes[426] = "Upgrade Required";

  // Server Error 5xx
  _error_codes[500] = "Internal Server Error";
  _error_codes[501] = "Not Implemented";
  _error_codes[502] = "Bad Gateway";
  _error_codes[503] = "Service Unavailable";
  _error_codes[504] = "Gateway Timeout";
  _error_codes[505] = "HTTP Version Not Supported";
}

std::string &StatusCode::operator[](int status_code) {
  return _error_codes[status_code];
}