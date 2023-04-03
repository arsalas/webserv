#include "Errors.hpp"
#include "logs/Log.hpp"
#include "Utils/Strings.hpp"

Errors::Errors(int code, std::string _message) : _message(_message), _code(code) {}

const char *Errors::what() const throw()
{
	Log::Error(Strings::intToString(_code) + ": " + _message);
	return _message.c_str();
}