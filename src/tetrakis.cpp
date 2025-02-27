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

// Helper functions for vector math.
inline Vertex subtract(const Vertex& a, const Vertex& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vertex cross(const Vertex& a, const Vertex& b) {
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

inline Vertex normalize(const Vertex& v) {
    float mag = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / mag, v.y / mag, v.z / mag };
}

void Tetrakis::calculateNormals() {
    for (int i = 0; i < 24; i++) {
        const Face &face = Tetrakis::faces[i];
        Vertex v1 = Tetrakis::vertices[face.vertex1];
        Vertex v2 = Tetrakis::vertices[face.vertex2];
        Vertex v3 = Tetrakis::vertices[face.vertex3];

        // Calculate the edge vectors.
        Vertex v21 = subtract(v2, v1);
        Vertex v32 = subtract(v3, v2);

        // Compute the face normal via the cross product and normalize it.
        Tetrakis::faceNormals[i] = normalize(cross(v21, v32));
    }
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