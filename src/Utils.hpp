#pragma once
#include <iostream>

class Utils
{
	static std::string rtrim(const std::string &s);
	static std::string ltrim(const std::string &s);

  public:
  static std::string	trim(const std::string &s);
  static std::string    split(std::string str, std::string del);
};
