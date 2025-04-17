#include <iostream>
#include <math.h>
#include "rasterizer.hpp"
#include "renderer.hpp"
#include "scene.hpp"

/*
Local/Model Space (vertex data)
↓ (model matrix)
World Space
↓ (view matrix) <----- your view / fpsview functions do this
Camera (View) Space
↓ (perspective projection matrix) <---- your perspective(...) does this
Clip Space (4D coordinates)
↓ (perspective divide by w coordinate, GPU automatic)
Normalized Device Coordinates (NDC, range [-1, 1])
↓ (viewport transform, GPU automatic)
Screen Space (pixels)
*/


void Renderer::drawScene(Scene& scene, float zNear, float zFar, float viewAngle) {

    prepareFrame(scene, zNear, zFar, viewAngle);
    for (auto& solidPtr : scene.solids) {
        drawRenderable(*solidPtr, scene);
    }
}

void Renderer::prepareFrame(Scene& scene, float zNear, float zFar, float viewAngle) {

    std::fill_n(scene.pixels, scene.screen.width * scene.screen.height, 0);
    scene.zBuffer->Clear(); // Clear the zBuffer

    //float zNear = 0.1f; // Near plane distance
    //float zFar  = 10000.0f; // Far plane distance
    float aspectRatio = scene.screen.width / scene.screen.height; // Width / Height ratio
    float fovRadians = viewAngle * (PI / 180.0f);

    scene.projectionMatrix = smath::perspective(zFar, zNear, aspectRatio, fovRadians);
}

void Renderer::drawRenderable(Solid& solid, Scene& scene) {

    prepareRenderable(solid, scene);
    Rasterizer rasterizer(&solid);
    rasterizer.projectRotateAllPoints(scene);
    rasterizer.addFaces(scene);
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
    slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
    slib::mat4 scale = smath::scale(slib::vec3({solid.position.zoom, solid.position.zoom, solid.position.zoom}));
    scene.fullTransformMat = translate * rotate * scale;
    scene.normalTransformMat = rotate;
}






