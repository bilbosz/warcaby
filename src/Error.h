#pragma once

#include <string>

class Error
{
public:
	Error();
	Error(std::string message);
	virtual ~Error();

	std::string getMessage() const;

protected:
	std::string message;
};