#include "Error.h"

Error::Error() : message("Unknown error") {}

Error::Error(std::string message) : message(message) {}

Error::~Error() {}

std::string Error::getMessage() const
{
    return message;
}
