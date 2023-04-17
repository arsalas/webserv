#include <iostream>
#include <sstream>
#include "Log.hpp"
#include "Utils/Colors.hpp"

void Log::Message(std::string out)
{
	std::cout << out << std::endl;
}

void Log::Info(std::string out)
{
	std::cout << BLU "" << out << "\n" RESET;
}

void Log::Error(std::string out)
{
	std::cout << RED "" << out << "\n" RESET;
}

void Log::Success(std::string out)
{
	std::cout << GRN "" << out << "\n" RESET;
}

void Log::Warning(std::string out)
{
	std::cout << YEL "" << out << "\n" RESET;
}
