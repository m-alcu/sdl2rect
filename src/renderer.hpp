#pragma once
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene, float zNear, float zFar, float viewAngle);
        void prepareFrame(Scene& scene, float zNear, float zFar, float viewAngle);
        void prepareRenderable(const Solid& solid, Scene& scene);
        vertex* projectRotateAllPoints(Solid& solid, const Scene& scene);            
        void drawRenderable(Solid& solid, Scene& scene);
        void addFaces(vertex *projectedPoints, const Solid& solid, Scene& scene);
};


