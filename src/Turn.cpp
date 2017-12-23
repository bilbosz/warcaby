#include "Turn.h"
#include "NotImplementedError.h"

Turn::Turn() : stepList(), capturesList(), capturesNumber(0U) {}

Turn::~Turn() {}

void Turn::reset()
{
    stepList.clear();
    capturesList.clear();
    capturesNumber = 0U;
}

std::string Turn::getInterpretation() const
{
    throw NotImplementedError();
}

uint16_t Turn::getCapturesNumber() const
{
    return capturesNumber;
}

bool Turn::operator<(const Turn &that) const
{
    if (capturesNumber < that.capturesNumber)
        return true;
    if (capturesNumber > that.capturesNumber)
        return false;
    return stepList < that.stepList;
}
