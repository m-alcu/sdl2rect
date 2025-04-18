#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:
        void drawScene(Scene& scene, float zNear, float zFar, float viewAngle, uint32_t* back);
        void prepareFrame(Scene& scene, float zNear, float zFar, float viewAngle, uint32_t* back);
        void prepareRenderable(const Solid& solid, Scene& scene);           
        void drawRenderable(Solid& solid, Scene& scene);
};


