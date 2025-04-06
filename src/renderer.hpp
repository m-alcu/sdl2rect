#pragma once
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene, float zNear, float zFar, float viewAngle);
        slib::vec4 projectedPoint(slib::vec3 point, int16_t i, const Scene& scene);
        vertex screenPoint(slib::vec3 point, slib::vec3 normal, slib::vec4 projectedPoint, int16_t i, const Scene& scene);
        void prepareScene(Scene& scene, float zNear, float zFar, float viewAngle);
        void prepareRenderable(const Solid& solid, Scene& scene);
        vertex* projectRotateAllPoints(Solid& solid, const Scene& scene);            
        void drawRenderable(Solid& solid, Scene& scene);
        void addFaces(vertex *projectedPoints, const Solid& solid, Scene& scene);
};


