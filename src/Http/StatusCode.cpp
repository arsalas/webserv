#include "StatusCode.hpp"

StatusCode::StatusCode()
{
	// Informational 1xx
	_codes[100] = "Continue";
	_codes[101] = "Switching Protocols";

	// Successful 2xx
	_codes[200] = "OK";
	_codes[201] = "Created";
	_codes[202] = "Accepted";
	_codes[203] = "Non-Authoritative Information";
	_codes[204] = "No Content";
	_codes[205] = "Reset Content";
	_codes[206] = "Partial Content";

	// Redirection 3xx
	_codes[300] = "Multiple Choices";
	_codes[301] = "Moved Permanently";
	_codes[302] = "Found";
	_codes[303] = "See Other";
	_codes[304] = "Not Modified";
	_codes[305] = "Use Proxy";
	_codes[307] = "Temporary Redirect";

	// Client Error 4xx
	_codes[400] = "Bad Request";
	_codes[401] = "Unauthorized";
	_codes[402] = "Payment Required";
	_codes[403] = "Forbidden";
	_codes[404] = "Not Found";
	_codes[405] = "Method Not Allowed";
	_codes[406] = "Not Acceptable";
	_codes[407] = "Proxy Authentication Required";
	_codes[408] = "Request Timeout";
	_codes[409] = "Conflict";
	_codes[410] = "Gone";
	_codes[411] = "Length Required";
	_codes[412] = "Precondition Failed";
	_codes[413] = "Payload Too Large";
	_codes[414] = "URI Too Long";
	_codes[415] = "Unsupported Media Type";
	_codes[416] = "Range Not Satisfiable";
	_codes[417] = "Expectation Failed";
	_codes[426] = "Upgrade Required";

	// Server Error 5xx
	_codes[500] = "Internal Server Error";
	_codes[501] = "Not Implemented";
	_codes[502] = "Bad Gateway";
	_codes[503] = "Service Unavailable";
	_codes[504] = "Gateway Timeout";
	_codes[505] = "HTTP Version Not Supported";
}

std::string &StatusCode::operator[](int code)
{
	return _codes[code];
}