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

    prepareScene(scene, zNear, zFar, viewAngle);
    for (auto& solidPtr : scene.solids) {
        drawRenderable(*solidPtr, scene);
    }
}

void Renderer::prepareScene(Scene& scene, float zNear, float zFar, float viewAngle) {

    std::fill_n(scene.pixels, scene.screen.width * scene.screen.height, 0);
    std::fill_n(
        scene.zBuffer,
        scene.screen.width * scene.screen.height,
        std::numeric_limits<float>::max() // Initialize zBuffer to the maximum float value
    );

    //float zNear = 0.1f; // Near plane distance
    //float zFar  = 10000.0f; // Far plane distance
    float aspectRatio = scene.screen.width / scene.screen.height; // Width / Height ratio
    float fovRadians = viewAngle * (PI / 180.0f);

    scene.projectionMatrix = smath::perspective(zFar, zNear, aspectRatio, fovRadians);
}

void Renderer::drawRenderable(Solid& solid, Scene& scene) {

    prepareRenderable(solid, scene);
    vertex * projectedPoints = projectRotateAllPoints(solid, scene);
    addFaces(projectedPoints, solid, scene);
    delete[] projectedPoints;
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
    slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
    slib::mat4 scale = smath::scale(slib::vec3({solid.position.zoom, solid.position.zoom, solid.position.zoom}));
    scene.fullTransformMat = translate * rotate * scale;
    scene.normalTransformMat = rotate;
}

vertex* Renderer::projectRotateAllPoints(Solid& solid, const Scene& scene) {
    // Allocate an array of Pixels on the heap
    vertex* screenPoints = new vertex[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {

        screenPoints[i].vtx = i;
        slib::vec4 point = scene.fullTransformMat * slib::vec4(solid.vertices[i], 1);
        screenPoints[i].vertexPoint = point * scene.projectionMatrix;
        screenPoints[i].normal = scene.normalTransformMat * slib::vec4(solid.vertexNormals[i], 0);
        screenPoints[i].ds = std::max(0.0f, smath::dot(screenPoints[i].normal, scene.lux)); // Calculate the dot product with the light direction
        screenPoints[i].p_x = (int32_t) ((screenPoints[i].vertexPoint.x / screenPoints[i].vertexPoint.w + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
        screenPoints[i].p_y = (int32_t) ((screenPoints[i].vertexPoint.y / screenPoints[i].vertexPoint.w + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
        screenPoints[i].p_z = screenPoints[i].vertexPoint.z / screenPoints[i].vertexPoint.w; // Store the depth value in the z-buffer
        }
    return screenPoints;
}

void Renderer::addFaces(vertex *projectedPoints, const Solid& solid, Scene& scene) {

    Rasterizer rasterizer(&solid, scene.pixels, scene.zBuffer);

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.

        triangle tri(projectedPoints[solid.faces[i].vertex1], projectedPoints[solid.faces[i].vertex2], projectedPoints[solid.faces[i].vertex3], i);

        if (rasterizer.visible(tri) && rasterizer.zFrustrum(tri) && !rasterizer.screenOutside(scene, tri)) {
            rasterizer.addTriangle(std::make_unique<triangle>(tri));
        }
    }
    
    for (auto& trianglePtr : rasterizer.triangles) {
        rasterizer.draw(*trianglePtr, solid, scene);
    }

}


