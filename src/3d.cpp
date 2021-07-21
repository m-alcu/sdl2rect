#include <iostream>
#include <math.h>
#include "poly.h"

void Matrix::init(int16_t xAngle, int16_t yAngle, int16_t zAngle) {

    double sinX = sin (xAngle);
    double cosX = cos (xAngle);
    double sinY = sin (yAngle);
    double cosY = cos (yAngle);
    double sinZ = sin (zAngle);
    double cosZ = cos (zAngle);

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

Pixel Project::proj3to2D(Vertex vertex, Screen screen) {

    Pixel pixel;
    pixel.x = (int16_t) ((256 * vertex.x) / (vertex.z+200000)) + screen.width / 2;
    pixel.y = (int16_t) ((256 * vertex.y) / (vertex.z+200000)) + screen.high / 2;
    return pixel;

}