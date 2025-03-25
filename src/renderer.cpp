#include <iostream>
#include <math.h>
#include "poly.hpp"
#include "renderer.hpp"
#include "scene.hpp"

void Renderer::drawScene(Scene& scene) {

    prepareScene(scene);
    for (auto& solidPtr : scene.solids) {
        drawRenderable(*solidPtr, scene);
    }
}

void Renderer::prepareScene(Scene& scene) {

    std::fill_n(scene.pixels, scene.screen.width * scene.screen.high, 0);
    std::fill_n(
        scene.zBuffer,
        scene.screen.width * scene.screen.high,
        std::numeric_limits<float>::max()
    );
}

void Renderer::drawRenderable(Solid& solid, Scene& scene) {

    prepareRenderable(solid, scene);
    vertex * projectedPoints = projectRotateAllPoints(solid, scene);
    slib::vec3 * rotatedVertexNormals = rotateVertexNormals(solid, scene);
    drawFaces(projectedPoints, solid, scene, rotatedVertexNormals);
    delete[] projectedPoints;
    delete[] rotatedVertexNormals;
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    scene.rotate = scene.rotate.init(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
}

vertex Renderer::proj3to2D(slib::vec3 point, Screen screen, Position position, int16_t i) {

    vertex pixel;
    pixel.p_x = (int16_t) ((position.zoom * (point.x + position.x)) / (point.z + position.z)) + screen.width / 2;
    pixel.p_y = (int16_t) ((position.zoom * (point.y + position.y)) / (point.z + position.z)) + screen.high / 2;
    pixel.p_z = point.z + position.z;
    pixel.vtx = i;
    return pixel;
}

slib::vec3* Renderer::rotateVertexNormals(const Solid& solid, const Scene& scene) {

    slib::vec3* rNormals = new slib::vec3[solid.numVertices];
    for (int i = 0; i < solid.numVertices; i++) {
        rNormals[i] = scene.rotate * solid.vertexNormals[i];
    }
    return rNormals;
}


vertex* Renderer::projectRotateAllPoints(Solid& solid, const Scene& scene) {
    // Allocate an array of Pixels on the heap
    vertex* projectedPoints = new vertex[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {
        projectedPoints[i] = proj3to2D(scene.rotate * solid.vertices[i], scene.screen, solid.position, i);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Renderer::drawFaces(vertex *projectedPoints, const Solid& solid, Scene& scene, slib::vec3 *rotatedVertexNormals) {

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.
        Rasterizer triangle(&solid, scene.pixels, scene.zBuffer);
        triangle.p1 = projectedPoints[solid.faces[i].vertex1];
        triangle.p2 = projectedPoints[solid.faces[i].vertex2];
        triangle.p3 = projectedPoints[solid.faces[i].vertex3];

        if (triangle.visible() && !triangle.outside(scene) && !triangle.behind()) {
            triangle.draw(solid, scene, solid.faces[i], solid.faceNormals[i], rotatedVertexNormals);
        }
    }                        
}
