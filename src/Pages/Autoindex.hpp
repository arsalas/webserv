#pragma once

#include <iostream>
#include "APage.hpp"

class Autoindex : public APage
{

public:
	Autoindex();
	virtual ~Autoindex();

	virtual std::string toStr() const;
};
