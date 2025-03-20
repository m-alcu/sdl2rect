#pragma once


#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Renderer {

    public:
        Pixel proj3to2D(Vec3 vertex, Screen screen, Position position, int16_t i);
        void prepareScene(const Solid& solid, Scene& scene);
        Pixel* projectRotateAllPoints(const Solid& solid, const Scene& scene);     
        Vec3* rotateNormals(const Solid& solid, const Scene& scene);          
        void drawObject(const Solid& solid, Scene& scene);
        void drawFaces(Pixel *projectedPoints, const Solid& solid, Scene& scene, Vec3 *rotatedNormals);
};


