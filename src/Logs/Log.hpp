#pragma once
#include <iostream>
#include <sstream>

class Log
{
public:
	static void Message(std::string out);
	static void Info(std::string out);
	static void Error(std::string out);
	static void Success(std::string out);
	static void Warning(std::string out);
};
