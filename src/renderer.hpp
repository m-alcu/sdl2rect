#pragma once
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene);
        slib::vec4 projectedPoint(slib::vec3 point, int16_t i, const Scene& scene);
        vertex screenPoint(slib::vec3 point, slib::vec3 normal, slib::vec4 projectedPoint, int16_t i, const Scene& scene);
        void prepareScene(Scene& scene);
        void prepareRenderable(const Solid& solid, Scene& scene);
        vertex* projectRotateAllPoints(Solid& solid, const Scene& scene);            
        void drawRenderable(Solid& solid, Scene& scene);
        void addFaces(vertex *projectedPoints, const Solid& solid, Scene& scene);
        void orderVerticesX(vertex *p1, vertex *p2, vertex *p3);
        void divideTriangle(triangle& tri, Rasterizer& rasterizer, Scene& scene, const Face& face);
        vertex gradientDy(vertex p1, vertex p2, slib::vec3& lux, const Face& face);
};


