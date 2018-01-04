#include "Error.h"

Error::Error() : message("Nieznany błąd") {}

Error::Error(std::string message) : message(message) {}

Error::~Error() {}

std::string Error::getMessage() const
{
    return message;
}
