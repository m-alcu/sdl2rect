#pragma once
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene);
        Pixel proj3to2D(slib::vec3 vertex, Screen screen, Position position, int16_t i);
        void prepareScene(Scene& scene);
        void prepareRenderable(const Solid& solid, Scene& scene);
        Pixel* projectRotateAllPoints(const Solid& solid, const Scene& scene);     
        slib::vec3* rotateVertexNormals(const Solid& solid, const Scene& scene);          
        void drawRenderable(const Solid& solid, Scene& scene);
        void drawFaces(Pixel *projectedPoints, const Solid& solid, Scene& scene, slib::vec3 *rotatedNormals);
};


