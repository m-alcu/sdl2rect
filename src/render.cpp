#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>
#include "poly.hpp"
#include "render.hpp"

void Render::drawObject(const Solid& solid, Scene& scene) {

    prepareScene(solid, scene);
    Pixel * projectedPoints = projectRotateAllPoints(solid, scene);
    Vec3 * rotatedNormals = rotateNormals(solid, scene);
    drawFaces(projectedPoints, solid, scene, rotatedNormals);
    delete[] projectedPoints;
    delete[] rotatedNormals;
}

void Render::prepareScene(const Solid& solid, Scene& scene) {

    memset(scene.pixels, 0, scene.screen.width * scene.screen.high * sizeof(uint32_t));
    std::copy(scene.zBufferInit, scene.zBufferInit + (scene.screen.width * scene.screen.high), scene.zBuffer);
    scene.rotate = scene.rotate.init(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    scene.inverseRotate = scene.inverseRotate.initInverse(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    scene.luxInversePrecomputed = scene.inverseRotate * scene.lux;
    scene.eyeInversePrecomputed = scene.inverseRotate * scene.eye;

}

Pixel Render::proj3to2D(Vec3 point, Screen screen, Position position, int16_t i) {

    Pixel pixel;
    pixel.p_x = (int16_t) ((position.zoom * (point.x + position.x)) / (point.z + position.z)) + screen.width / 2;
    pixel.p_y = (int16_t) ((position.zoom * (point.y + position.y)) / (point.z + position.z)) + screen.high / 2;
    pixel.p_z = (int64_t) (point.z + position.z) << 32;
    pixel.vtx = i;
    return pixel;
}

Vec3* Render::rotateNormals(const Solid& solid, const Scene& scene) {

    if (scene.shading != Shading::Precomputed) {
        return nullptr;
    }

    Vec3* rNormals = new Vec3[solid.numVertices];
    for (int i = 0; i < solid.numVertices; i++) {
        rNormals[i] = scene.rotate * solid.vertexNormals[i];
    }
    return rNormals;
}


Pixel* Render::projectRotateAllPoints(const Solid& solid, const Scene& scene) {
    // Allocate an array of Pixels on the heap
    Pixel* projectedPoints = new Pixel[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {
        projectedPoints[i] = proj3to2D(scene.rotate * solid.vertices[i], scene.screen, solid.position, i);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Render::drawFaces(Pixel *projectedPoints, const Solid& solid, Scene& scene, Vec3 *rotatedNormals) {

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.
        Triangle triangle(&solid, scene.pixels, scene.zBuffer);
        triangle.p1 = projectedPoints[solid.faces[i].vertex1];
        triangle.p2 = projectedPoints[solid.faces[i].vertex2];
        triangle.p3 = projectedPoints[solid.faces[i].vertex3];

        if (triangle.visible() && !triangle.outside(scene) && !triangle.behind()) {
            triangle.draw(solid, scene, solid.faces[i], solid.faceNormals[i], rotatedNormals);
        }
    }                        
}
