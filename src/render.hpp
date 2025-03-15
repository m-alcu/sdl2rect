#pragma once


#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Render {

    public:
        Pixel proj3to2D(Vec3 vertex, Screen screen, Position position, int16_t i);
        Pixel* projectRotateAllPoints(const Solid& solid, const Screen& screen, const Matrix& matrix, Position position);        
        void drawObject(const Solid& solid, uint32_t *pixels, Screen screen, int64_t *zBuffer, Vec3 lux, Shading shading);
        void drawFaces(Pixel *projectedPoints, Screen screen, uint32_t *pixels,  int64_t *zBuffer, Vec3 lux, Shading shading, const Solid& solid, Matrix inverseMatrix);
};


