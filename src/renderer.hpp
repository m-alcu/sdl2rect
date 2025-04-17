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
        void projectRotateAllPoints(Solid& solid, const Scene& scene, Rasterizer& rasterizer);            
        void drawRenderable(Solid& solid, Scene& scene);
};


