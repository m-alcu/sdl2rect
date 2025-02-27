#include <iostream>
#include <math.h>
#include "poly.hpp"


void Tetrakis::loadVertices() {
    const float half = 32768.f;
    const float axisDist = half * std::sqrt(3.f);  // ≈ 56755

    int index = 0;
    // Generate the 8 cube vertices with explicit sign choices
    for (int xSign : {1, -1}) {
        for (int ySign : {1, -1}) {
            for (int zSign : {1, -1}) {
                Tetrakis::vertices[index++] = { half * xSign, half * ySign, half * zSign };
            }
        }
    }

    // Generate the 6 axis-aligned vertices
    Tetrakis::vertices[index++] = {  axisDist, 0,         0 };
    Tetrakis::vertices[index++] = {  0,         axisDist, 0 };
    Tetrakis::vertices[index++] = {  0,         0,         axisDist };
    Tetrakis::vertices[index++] = { -axisDist, 0,         0 };
    Tetrakis::vertices[index++] = {  0,        -axisDist, 0 };
    Tetrakis::vertices[index++] = {  0,         0,        -axisDist };
}

void Tetrakis::loadFaces() {
    // Define the quadrilaterals (outer vertices) and centers for each face group.
    const uint16_t quads[6][4] = {
        {2, 0, 1, 3},  // group 0, center 8
        {4, 5, 1, 0},  // group 1, center 9
        {2, 6, 4, 0},  // group 2, center 10
        {4, 6, 7, 5},  // group 3, center 11
        {7, 6, 2, 3},  // group 4, center 12
        {1, 5, 7, 3}   // group 5, center 13
    };
    const uint16_t centers[6] = {8, 9, 10, 11, 12, 13};

    // There are 6 groups, each generating 4 faces (triangles) = 24 total faces.
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            int faceIndex = i * 4 + j;
            // Directly assign values to the face.
            Tetrakis::faces[faceIndex].color   = ((j % 2 == 0) ? 0xff0058fc : 0Xffffffff);
            Tetrakis::faces[faceIndex].vertex1 = quads[i][j];
            Tetrakis::faces[faceIndex].vertex2 = quads[i][(j + 1) % 4]; // wrap-around for the quad
            Tetrakis::faces[faceIndex].vertex3 = centers[i];
        }
    }
}