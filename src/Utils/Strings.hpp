#pragma once

#include <iostream>
#include <vector>
#include "File.hpp"

class Strings
{
public:
	static const std::string intToString(int);
	static std::string trim(const std::string &s);
	static std::string trim(const std::string &s, const std::string &delim);
	static std::vector<std::string> vectorSplit(std::string file, std::string delimiter);
	static std::string deleteQuotes(std::string str);
	static std::string rtrim(const std::string &s, const std::string &delim);
	static std::string ltrim(const std::string &s, const std::string &delim);

private:
	Strings();

public:
	class myException : virtual public std::exception
	{
	public:
		myException(std::string errMsg, int errorCode = 0)
		{
			_errorMsg = errMsg;
			_errorCode = errorCode;
			if (_errorCode)
			{
				_errorMsg = _errorMsg + " (" + (std::to_string(_errorCode)) + ": " + strerror(_errorCode) + ")";
			}
		}
		virtual ~myException() throw() {}

		const char *what() const throw()
		{
			return (_errorMsg.c_str());
		}

	private:
		int _errorCode;
		std::string _errorMsg;
	};
};
