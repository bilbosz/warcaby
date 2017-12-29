#pragma once

#include "Error.h"

class NotImplementedError final : public Error
{
public:
    NotImplementedError();
    virtual ~NotImplementedError();
};