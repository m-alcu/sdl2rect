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

void Render::projectAll2DPoints(Vertex *vertices, Pixel *projectedPoints, Screen screen) {

    for (int i=0; i<14; i++) {
        projectedPoints[i] = proj3to2D(vertices[i], screen);
    }

}

void Render::rotateAllVertices(Vertex *vertices, Vertex *rotatedVertices, Matrix matrix) {

    for (int i=0; i<14; i++) {
        rotatedVertices[i] = matrix * vertices[i];
    }

}

void Render::drawAllFaces(Face *faces, Pixel *projectedPoints, Vertex *faceNormals, Screen screen, uint32_t *pixels, Matrix matrix) {

     for (int i=0; i<24; i++) {
         drawFace(faces[i], projectedPoints, faceNormals[i], screen, pixels, matrix);
     }
}


void Render::drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels, Matrix matrix) {

    Triangle triangle(pixels, screen);

    Vertex lux;

    lux.x = 0;
    lux.y = 0;
    lux.z = 1;

    triangle.p1.x = projectedPoints[face.vertex1].x;
    triangle.p1.y = projectedPoints[face.vertex1].y;
    triangle.p2.x = projectedPoints[face.vertex2].x;
    triangle.p2.y = projectedPoints[face.vertex2].y;
    triangle.p3.x = projectedPoints[face.vertex3].x;
    triangle.p3.y = projectedPoints[face.vertex3].y;

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

    Pixel * projectedPoints = new Pixel[14];
    Vertex * rotatedVertices = new Vertex[14];
    Matrix matrix = matrix.init(tetrakis.xAngle, tetrakis.yAngle, tetrakis.zAngle);
    rotateAllVertices(tetrakis.vertices, rotatedVertices, matrix);
    projectAll2DPoints(rotatedVertices, projectedPoints, screen);
    drawAllFaces(tetrakis.faces, projectedPoints, tetrakis.faceNormals, screen, pixels, matrix);

}