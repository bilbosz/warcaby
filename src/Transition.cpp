#include "Transition.h"

Transition::Transition(EasingCurve curve) : minValue(), maxValue(), lasting(), curve(curve), finished(true) {}

Transition::~Transition() {}

float Transition::evaluate(sf::Time time)
{
    if (time <= startTime) {
        return minValue;
    }
    if (time >= startTime + lasting) {
        finished = true;
        return maxValue;
    }
    float progress = (time - startTime) / lasting;
    float valueSpan = maxValue - minValue;
    return minValue + valueSpan * curve(progress);
}

void Transition::setMinValue(float value)
{
    minValue = value;
}

float Transition::getMinValue() const
{
    return minValue;
}

void Transition::setMaxValue(float value)
{
    maxValue = value;
}

float Transition::getMaxValue() const
{
    return maxValue;
}

void Transition::setStartTime(sf::Time time)
{
    finished = false;
    startTime = time;
}

void Transition::setLasting(sf::Time time)
{
    lasting = time;
}

sf::Time Transition::getLasting() const
{
    return lasting;
}

bool Transition::isFinished() const
{
    return finished;
}

void Transition::setEasingCurve(EasingCurve curve)
{
    this->curve = curve;
}
