#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include "objects/solid.hpp"
#include "rasterizer.hpp"
#include "effects/flatEffect.hpp"
#include "effects/GouraudEffect.hpp"
#include "effects/BlinnPhongEffect.hpp"
#include "effects/PhongEffect.hpp"
#include "effects/TexturedFlatEffect.hpp"
#include "effects/TexturedGouraudEffect.hpp"
#include "effects/TexturedPhongEffect.hpp"
#include "effects/TexturedBlinnPhongEffect.hpp"

class Renderer {

    public:

        void drawScene(Scene& scene, float zNear, float zFar, float viewAngle, uint32_t* back) {

            prepareFrame(scene, zNear, zFar, viewAngle, back);
            for (auto& solidPtr : scene.solids) {
                switch (solidPtr->shading) {
                    case Shading::Flat: 
                        flatRasterizer.drawRenderable(*solidPtr, scene);
                        break;   
                    case Shading::TexturedFlat: 
                        texturedFlatRasterizer.drawRenderable(*solidPtr, scene);
                        break;                             
                    case Shading::Gouraud: 
                        gouraudRasterizer.drawRenderable(*solidPtr, scene);
                        break;
                    case Shading::TexturedGouraud: 
                        texturedGouraudRasterizer.drawRenderable(*solidPtr, scene);
                        break;                        
                    case Shading::BlinnPhong:
                        blinnPhongRasterizer.drawRenderable(*solidPtr, scene);
                        break;  
                    case Shading::TexturedBlinnPhong:
                        texturedBlinnPhongRasterizer.drawRenderable(*solidPtr, scene);
                        break;                                                       
                    case Shading::Phong:
                        phongRasterizer.drawRenderable(*solidPtr, scene);
                        break;      
                    case Shading::TexturedPhong:
                        texturedPhongRasterizer.drawRenderable(*solidPtr, scene);
                        break;                                             
                    default: flatRasterizer.drawRenderable(*solidPtr, scene);
                }
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
        
        Rasterizer<FlatEffect> flatRasterizer;
        Rasterizer<GouraudEffect> gouraudRasterizer;
        Rasterizer<PhongEffect> phongRasterizer;
        Rasterizer<BlinnPhongEffect> blinnPhongRasterizer;
        Rasterizer<TexturedFlatEffect> texturedFlatRasterizer;
        Rasterizer<TexturedGouraudEffect> texturedGouraudRasterizer;
        Rasterizer<TexturedPhongEffect> texturedPhongRasterizer;
        Rasterizer<TexturedBlinnPhongEffect> texturedBlinnPhongRasterizer;
};


