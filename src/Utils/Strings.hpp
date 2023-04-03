#pragma once

#include <iostream>
#include <vector>

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
		myException(std::string msg, int err_num = 0, std::string arg = "") : errno_(err_num)
		{
			error_msg_ = msg;
			if (error_msg_.find('%') != std::string::npos)
				error_msg_.replace(error_msg_.find('%'), 1, arg);
			if (errno_)
			{
				error_msg_ = error_msg_ + " (" + ft::to_string(errno_) + ": " + strerror(errno_) + ")";
			}
		}
		virtual ~myException() throw() {}

		const char *what() const throw()
		{
			return error_msg_.c_str();
		}

	private:
		int errno_;
		std::string error_msg_;
	};
};
