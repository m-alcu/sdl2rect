#pragma once
#include <cstdint>

template<class V>
class Triangle
{
public:
    V p1, p2, p3;
    V edge12, edge23, edge13;
    int16_t i;
    Triangle(const Triangle& _t) : p1(_t.p1), p2(_t.p2), p3(_t.p3), i(_t.i) {};
    Triangle(const V& _p1, const V& _p2, const V& _p3, int16_t _i) : p1(_p1), p2(_p2), p3(_p3), i(_i) {};
};


