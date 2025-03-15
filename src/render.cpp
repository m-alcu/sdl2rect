#include <iostream>
#include <math.h>
#include "poly.hpp"
#include "render.hpp"

void Render::drawObject(const Solid& solid, uint32_t *pixels, int64_t *zBuffer, Scene scene) {

    scene.rotate = scene.rotate.init(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    scene.inverseRotate = scene.inverseRotate.initInverse(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    Pixel * projectedPoints = projectRotateAllPoints(solid, scene);
    drawFaces(projectedPoints, pixels, zBuffer, solid, scene);
    delete[] projectedPoints;
}

Pixel Render::proj3to2D(Vec3 vertex, Screen screen, Position position, int16_t i) {

    Pixel pixel;
    pixel.p_x = (int16_t) ((position.zoom * (vertex.x + position.x)) / (vertex.z + position.z)) + screen.width / 2;
    pixel.p_y = (int16_t) ((position.zoom * (vertex.y + position.y)) / (vertex.z + position.z)) + screen.high / 2;
    pixel.p_z = (int64_t) (vertex.z + position.z) << 32;
    pixel.vtx = i;
    return pixel;
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

void Render::drawFaces(Pixel *projectedPoints, uint32_t *pixels, int64_t *zBuffer, const Solid& solid, Scene scene) {

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.
        Triangle triangle(&solid, pixels, zBuffer, scene.screen);
        triangle.p1 = projectedPoints[solid.faces[i].vertex1];
        triangle.p2 = projectedPoints[solid.faces[i].vertex2];
        triangle.p3 = projectedPoints[solid.faces[i].vertex3];
        triangle.shading = scene.shading;

        if (triangle.visible() && !triangle.outside() && !triangle.behind()) {
            if (scene.shading == Shading::Flat) {
                int32_t bright = (int32_t) (std::max(0.0f, solid.faceNormals[i].dot(scene.inverseRotate * scene.lux)) * 65536);
                triangle.color = RGBValue(solid.faces[i].material.Ambient, bright).bgra_value;
            } else {
                triangle.color = solid.faces[i].material.Ambient;
            }
            triangle.draw(solid, scene.inverseRotate * scene.lux, solid.faces[i]);
        }
    }                        
}
