#pragma once

#include <iostream>
#include "APage.hpp"

class ServerError : public APage
{

public:
	ServerError();
	virtual ~ServerError();

	virtual std::string toStr() const;
};
