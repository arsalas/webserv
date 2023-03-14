#include "Strings.hpp"
#include <sstream>

const std::string Strings::intToString(int number)
{
	std::ostringstream ss;
	ss << number;
	return ss.str();
}