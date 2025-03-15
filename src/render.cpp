#include <iostream>
#include <math.h>
#include "poly.hpp"
#include "render.hpp"

void Render::drawObject(const Solid& solid, uint32_t *pixels, Screen screen, int64_t *zBuffer, Vec3 lux, Shading shading) {

    Matrix matrix = matrix.init(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    Matrix inverseMatrix = inverseMatrix.initInverse(solid.position.xAngle, solid.position.yAngle, solid.position.zAngle);
    Pixel * projectedPoints = projectRotateAllPoints(solid, screen, matrix, solid.position);
    drawFaces(projectedPoints, screen, pixels, zBuffer, lux, shading, solid, inverseMatrix);
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

Pixel* Render::projectRotateAllPoints(const Solid& solid, const Screen& screen, const Matrix& matrix, Position position) {
    // Allocate an array of Pixels on the heap
    Pixel* projectedPoints = new Pixel[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {
        projectedPoints[i] = proj3to2D(matrix * solid.vertices[i], screen, position, i);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Render::drawFaces(Pixel *projectedPoints, Screen screen, 
                        uint32_t *pixels, int64_t *zBuffer, Vec3 lux, 
                        Shading shading, const Solid& solid, Matrix inverseMatrix) {

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.
        Triangle triangle(&solid, pixels, zBuffer, screen);
        triangle.p1 = projectedPoints[solid.faces[i].vertex1];
        triangle.p2 = projectedPoints[solid.faces[i].vertex2];
        triangle.p3 = projectedPoints[solid.faces[i].vertex3];
        triangle.shading = shading;

        if (triangle.visible() && !triangle.outside() && !triangle.behind()) {
            if (shading == Shading::Flat) {
                int32_t bright = (int32_t) (std::max(0.0f, solid.faceNormals[i].dot(inverseMatrix * lux)) * 65536);
                triangle.color = RGBValue(solid.faces[i].material.Ambient, bright).bgra_value;
            } else {
                triangle.color = solid.faces[i].material.Ambient;
            }
            triangle.draw(solid, inverseMatrix * lux, solid.faces[i]);
        }
    }                        
}
