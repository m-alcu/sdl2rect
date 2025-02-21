#include <iostream>
#include <math.h>
#include "poly.h"

Matrix Matrix::init(float xAngle, float yAngle, float zAngle) {
    // Create individual rotation matrices
    Matrix Rx = {
        1,         0,          0,
        0, cos(xAngle), -sin(xAngle),
        0, sin(xAngle),  cos(xAngle)
    };

    Matrix Ry = {
         cos(yAngle), 0, sin(yAngle),
                 0, 1,         0,
        -sin(yAngle), 0, cos(yAngle)
    };

    Matrix Rz = {
        cos(zAngle), -sin(zAngle), 0,
        sin(zAngle),  cos(zAngle), 0,
                0,          0, 1
    };

    // Multiply matrices in the proper order (for example: R = Rz * Ry * Rx)
    Matrix R = Rz * Ry * Rx;  // Assuming you've overloaded the operator*

    return R;
}

Pixel Render::proj3to2D(Vertex vertex, Screen screen) {

    Pixel pixel;
    pixel.x = (int16_t) ((720 * vertex.x) / (vertex.z+200000)) + screen.width / 2;
    pixel.y = (int16_t) ((720 * vertex.y) / (vertex.z+200000)) + screen.high / 2;
    return pixel;

}

Pixel* Render::projectRotateAllPoints(const Tetrakis& tetrakis, const Screen& screen, const Matrix& matrix) {
    // Allocate an array of Pixels on the heap
    Pixel* projectedPoints = new Pixel[tetrakis.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < tetrakis.numVertices; i++) {
        projectedPoints[i] = proj3to2D(matrix * tetrakis.vertices[i], screen);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Render::drawAllFaces(Tetrakis tetrakis, Pixel *projectedPoints, Screen screen, uint32_t *pixels, Matrix matrix) {

     for (int i=0; i<tetrakis.numFaces; i++) {
         drawFace(tetrakis.faces[i], projectedPoints, tetrakis.faceNormals[i], screen, pixels, matrix);
     }
}


void Render::drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels, Matrix matrix) {

    Triangle triangle(pixels, screen);

    Vertex lux = {0,0,1};
    triangle.p1 = projectedPoints[face.vertex1];
    triangle.p2 = projectedPoints[face.vertex2];
    triangle.p3 = projectedPoints[face.vertex3];

    if (triangle.visible()) {

        Vertex rotatedNormal = matrix * faceNormal;

        float bright = lux.x*rotatedNormal.x + lux.y*rotatedNormal.y + lux.z*rotatedNormal.z;

        RGBValue color;
        color.long_value = face.color;
        color.rgba.red = (uint8_t) (color.rgba.red * bright);
        color.rgba.green = (uint8_t) (color.rgba.green * bright);
        color.rgba.blue = (uint8_t) (color.rgba.blue * bright); 
        triangle.color = color.long_value;

        triangle.draw();
    }

}



void Render::drawObject(Tetrakis tetrakis, uint32_t *pixels, Screen screen) {

    Matrix matrix = matrix.init(tetrakis.xAngle, tetrakis.yAngle, tetrakis.zAngle);
    Pixel * projectedPoints = projectRotateAllPoints(tetrakis, screen, matrix);
    drawAllFaces(tetrakis, projectedPoints, screen, pixels, matrix);
    delete[] projectedPoints;

}