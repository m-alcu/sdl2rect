#include <iostream>
#include <math.h>
#include "poly.h"

void Matrix::init(float xAngle, float yAngle, float zAngle) {

    float sinX = sin (xAngle);
    float cosX = cos (xAngle);
    float sinY = sin (yAngle);
    float cosY = cos (yAngle);
    float sinZ = sin (zAngle);
    float cosZ = cos (zAngle);

    Matrix::r00 = cosY*cosZ;
    Matrix::r01 = cosY*sinZ;
    Matrix::r02 = sinY;

    Matrix::r10 = -sinX*sinY*cosZ - cosX*sinZ;
    Matrix::r11 = -sinX*sinY*sinZ + cosX*cosZ;
    Matrix::r12 = sinX*cosY;

    Matrix::r20 = -cosX*sinY*cosZ + sinX*sinZ;
    Matrix::r21 = -cosX*sinY*sinZ - sinX*cosZ;
    Matrix::r22 = cosX*cosY;
}

Vertex Matrix::rotate(Vertex vertex) {

    Vertex result;
    result.x = Matrix::r00*vertex.x + Matrix::r01*vertex.y + Matrix::r02*vertex.z;
    result.y = Matrix::r10*vertex.x + Matrix::r11*vertex.y + Matrix::r12*vertex.z;
    result.z = Matrix::r20*vertex.x + Matrix::r21*vertex.y + Matrix::r22*vertex.z;
    return result;

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
        rotatedVertices[i] = matrix.rotate(vertices[i]);
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

        Vertex rotatedNormal = matrix.rotate(faceNormal);

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



void Render::drawObject(Face *faces, Vertex *vertices, Vertex *faceNormals, uint32_t *pixels, Screen screen, float xAngle, float yAngle, float zAngle) {

    Matrix matrix;
    Pixel * projectedPoints = new Pixel[14];
    Vertex * rotatedVertices = new Vertex[14];

    matrix.init(xAngle, yAngle, zAngle);
    rotateAllVertices(vertices, rotatedVertices, matrix);
    projectAll2DPoints(rotatedVertices, projectedPoints, screen);
    drawAllFaces(faces, projectedPoints, faceNormals, screen, pixels, matrix);

}