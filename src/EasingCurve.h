#pragma once

/** Funkcja  ci¹g³a, maj¹ca wartoœci w przedziale <0; 1>. W punkcie 0 ma wartoœæ 0, w punkcie 1 ma wartoœæ 1. */
class EasingCurve
{
public:
    // myœlê, ¿e nie trzeba robiæ na template'ach, nie trzeba u¿ywaæ tu klas
    typedef float (*UnaryFunctionType)(float);
    EasingCurve(UnaryFunctionType function);
    virtual ~EasingCurve();

    float operator()(float x) const;

private:
    UnaryFunctionType function;
};

extern EasingCurve Line;
extern EasingCurve Parabola;
extern EasingCurve ShrinkedSin;
