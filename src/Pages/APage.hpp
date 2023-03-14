#pragma once

#include <iostream>

class APage
{

public:
	APage();
	virtual ~APage();

	virtual std::string toStr() const = 0;
};