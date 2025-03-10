#include <iostream>
#include <math.h>
#include "poly.hpp"
#include "render.hpp"

inline float dotProduct(const Vertex& a, const Vertex& b) {
    return { a.x * b.x + a.y * b.y + a.z * b.z };
}

Pixel Render::proj3to2D(Vertex vertex, Vertex vertexNormal, Screen screen, Position position, Vertex lux) {

    Pixel pixel;
    pixel.p_x = (int16_t) ((position.zoom * (vertex.x + position.x)) / (vertex.z + position.z)) + screen.width / 2;
    pixel.p_y = (int16_t) ((position.zoom * (vertex.y + position.y)) / (vertex.z + position.z)) + screen.high / 2;
    pixel.v_z = (int32_t) vertex.z + position.z;
    pixel.s = std::max(0.0f,dotProduct(lux, vertexNormal));
    return pixel;
}

Pixel* Render::projectRotateAllPoints(const Solid& solid, const Screen& screen, const Matrix& matrix, Position position, Vertex lux) {
    // Allocate an array of Pixels on the heap
    Pixel* projectedPoints = new Pixel[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {
        projectedPoints[i] = proj3to2D(matrix * solid.vertices[i], matrix * solid.vertexNormals[i], screen, position, lux);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Render::drawAllFaces(const Solid& solid, Pixel *projectedPoints, Screen screen, uint32_t *pixels, Matrix matrix, int64_t *zBuffer, Vertex lux, Shading shading) {

     for (int i=0; i<solid.numFaces; i++) {
         drawFace(solid.faces[i], projectedPoints, solid.faceNormals[i], screen, pixels, matrix, zBuffer, lux, shading);
     }
}



void Render::drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels, Matrix matrix, int64_t *zBuffer, Vertex lux, Shading shading) {

    Triangle triangle(pixels, zBuffer, screen);
    triangle.p1 = projectedPoints[face.vertex1];
    triangle.p2 = projectedPoints[face.vertex2];
    triangle.p3 = projectedPoints[face.vertex3];
    triangle.shading = shading;

    if (triangle.visible() && !triangle.outside() && !triangle.behind()) {

        if (shading == Shading::Flat) {
            int32_t bright = (int32_t) (std::max(0.0f,dotProduct(lux, matrix * faceNormal)) * 65536); 
            triangle.color = RGBValue(face.color, bright).bgra_value;
        } else {
            triangle.color = face.color;
        }
        triangle.draw();
    }
}



void Render::drawObject(const Solid& solid, uint32_t *pixels, Screen screen, int64_t *zBuffer, Position position, Vertex lux, Shading shading) {

    Matrix matrix = matrix.init(position.xAngle, position.yAngle, position.zAngle);
    Pixel * projectedPoints = projectRotateAllPoints(solid, screen, matrix, position, lux);
    drawAllFaces(solid, projectedPoints, screen, pixels, matrix, zBuffer, lux, shading);
    delete[] projectedPoints;

}