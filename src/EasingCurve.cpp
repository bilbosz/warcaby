#include "EasingCurve.h"

#include <cmath>

EasingCurve::EasingCurve(EasingCurve::UnaryFunctionType function) : function(function) {}

EasingCurve::~EasingCurve() {}

float EasingCurve::operator()(float x) const
{
    return function(x);
}

float parabolaFun(float x)
{
    return x * x;
}

float shrinkedSinFun(float x)
{
    return sin(asin(1.0f) * x);
}

float lineFun(float x)
{
    return x;
}

EasingCurve Line(lineFun);
EasingCurve Parabola(parabolaFun);
EasingCurve ShrinkedSin(shrinkedSinFun);