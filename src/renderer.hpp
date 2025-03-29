#pragma once
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene);
        vertex proj3to2D(slib::vec3 vertex, Screen screen, Position position, int16_t i, const Scene& scene);
        void prepareScene(Scene& scene);
        void prepareRenderable(const Solid& solid, Scene& scene);
        vertex* projectRotateAllPoints(Solid& solid, const Scene& scene);     
        slib::vec3* rotateVertexNormals(const Solid& solid, const Scene& scene);          
        void drawRenderable(Solid& solid, Scene& scene);
        void drawFaces(vertex *projectedPoints, const Solid& solid, Scene& scene, slib::vec3 *rotatedNormals);
};


