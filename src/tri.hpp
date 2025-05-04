#pragma once
#include <cstdint>
#include "slib.hpp"
#include "objects/solid.hpp"

template<class V>
class Triangle
{
public:
    V p1, p2, p3;
    Face face;
    slib::vec3 faceNormal;
    uint32_t flatColor;
    Triangle(const Triangle& _t) : p1(_t.p1), p2(_t.p2), p3(_t.p3), face(_t.face), faceNormal(_t.faceNormal) {};
    Triangle(const V& _p1, const V& _p2, const V& _p3, Face _f, slib::vec3 _fn) : p1(_p1), p2(_p2), p3(_p3), face(_f), faceNormal(_fn) {};
};


