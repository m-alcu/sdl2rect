#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "rasterizer.hpp"

class Renderer {

    public:

        void drawScene(Scene& scene, float zNear, float zFar, float viewAngle, uint32_t* back) {

            prepareFrame(scene, zNear, zFar, viewAngle, back);
            for (auto& solidPtr : scene.solids) {
                drawRenderable(*solidPtr, scene);
            }
        }

        void prepareFrame(Scene& scene, float zNear, float zFar, float viewAngle, uint32_t* back) {

            //std::fill_n(scene.pixels, scene.screen.width * scene.screen.height, 0);
            auto* pixels = static_cast<uint32_t*>(scene.sdlSurface->pixels);
            std::copy(back, back + scene.screen.width * scene.screen.height, pixels);
            scene.zBuffer->Clear(); // Clear the zBuffer
        
            //float zNear = 0.1f; // Near plane distance
            //float zFar  = 10000.0f; // Far plane distance
            float aspectRatio = scene.screen.width / scene.screen.height; // Width / Height ratio
            float fovRadians = viewAngle * (PI / 180.0f);
        
            scene.projectionMatrix = smath::perspective(zFar, zNear, aspectRatio, fovRadians);
        }

        void drawRenderable(Solid& solid, Scene& scene) {

            prepareRenderable(solid, scene);
            rasterizer = Rasterizer(&solid);
            rasterizer.ProcessVertex(scene);
            rasterizer.DrawFaces(scene);
        }
        
        void prepareRenderable(const Solid& solid, Scene& scene) {
        
            slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
            slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
            slib::mat4 scale = smath::scale(slib::vec3({solid.position.zoom, solid.position.zoom, solid.position.zoom}));
            scene.fullTransformMat = translate * rotate * scale;
            scene.normalTransformMat = rotate;
        }

        Rasterizer rasterizer;
};


