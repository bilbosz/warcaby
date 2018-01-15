#pragma once

#include "EasingCurve.h"

#include <SFML/System.hpp>

class Transition
{
public:
    Transition(EasingCurve curve = Line);
    virtual ~Transition();

    float evaluate(sf::Time time);

    void setMinValue(float value);
    float getMinValue() const;
    void setMaxValue(float value);
    float getMaxValue() const;

    void setStartTime(sf::Time time);

    void setLasting(sf::Time time);
    sf::Time getLasting() const;

    bool isFinished() const;

    void setEasingCurve(EasingCurve curve);

private:
    float minValue, maxValue;
    sf::Time startTime, lasting;
    EasingCurve curve;
    bool finished;
};
