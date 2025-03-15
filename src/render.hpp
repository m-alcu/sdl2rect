#pragma once


#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Render {

    public:
        Pixel proj3to2D(Vec3 vertex, Screen screen, Position position, int16_t i);
        Pixel* projectRotateAllPoints(const Solid& solid, const Scene& scene, Position position);        
        void drawObject(const Solid& solid, uint32_t *pixels, int64_t *zBuffer, Scene scene);
        void drawFaces(Pixel *projectedPoints, uint32_t *pixels,  int64_t *zBuffer, const Solid& solid, Scene scene);
};


