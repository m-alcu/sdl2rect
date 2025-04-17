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
    projectRotateAllPoints(solid, scene, rasterizer);
    addFaces(scene, rasterizer);
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
    slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
    slib::mat4 scale = smath::scale(slib::vec3({solid.position.zoom, solid.position.zoom, solid.position.zoom}));
    scene.fullTransformMat = translate * rotate * scale;
    scene.normalTransformMat = rotate;
}

void Renderer::projectRotateAllPoints(Solid& solid, const Scene& scene, Rasterizer& rasterizer) {
    // Allocate an array of Pixels on the heap
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {
        vertex screenPoint;
        slib::vec4 point = scene.fullTransformMat * slib::vec4(solid.vertices[i], 1);
        screenPoint.vertexPoint = point * scene.projectionMatrix;
        screenPoint.normal = scene.normalTransformMat * slib::vec4(solid.vertexNormals[i], 0);
        screenPoint.ds = std::max(0.0f, smath::dot(screenPoint.normal, scene.lux)); // Calculate the dot product with the light direction
        screenPoint.p_x = (int32_t) ((screenPoint.vertexPoint.x / screenPoint.vertexPoint.w + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_y = (int32_t) ((screenPoint.vertexPoint.y / screenPoint.vertexPoint.w + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_z = screenPoint.vertexPoint.z / screenPoint.vertexPoint.w; // Store the depth value in the z-buffer
        rasterizer.addPoint(std::make_unique<vertex>(screenPoint)); // Add the point to the rasterizer
        }
}

void Renderer::addFaces(Scene& scene, Rasterizer& rasterizer) {


    for (int i=0; i<rasterizer.solid->numFaces; i++) {

        Triangle<vertex> tri(
            *rasterizer.projectedPoints[rasterizer.solid->faces[i].vertex1],
            *rasterizer.projectedPoints[rasterizer.solid->faces[i].vertex2],
            *rasterizer.projectedPoints[rasterizer.solid->faces[i].vertex3],
            i
        );

        if (rasterizer.visible(tri)) {
            rasterizer.ClipCullTriangle(std::make_unique<Triangle<vertex>>(tri));
        }
    }
    
    for (auto& trianglePtr : rasterizer.triangles) {
        rasterizer.draw(*trianglePtr, *rasterizer.solid, scene);
    }

}


