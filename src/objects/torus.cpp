#include <iostream>
#include <math.h>
#include "../poly.hpp"
#define M_PI 3.14159265358979323846

void Torus::loadVertices() {
}

void Torus::loadFaces() {
}

void Torus::setup(int uSteps, int vSteps, float R, float r) {
    loadVertices(uSteps, vSteps, R, r);
    loadFaces(uSteps, vSteps);
    calculateNormals();
    calculateVertexNormals();
}

void Torus::loadVertices(int uSteps, int vSteps, float R, float r) {

    for (int i = 0; i < uSteps; i++) {
        float u = i * 2 * M_PI / uSteps;
        float cosU = cos(u);
        float sinU = sin(u);
        for (int j = 0; j < vSteps; j++) {
            float v = j * 2 * M_PI / vSteps;
            float cosV = cos(v);
            float sinV = sin(v);
            float x = (R + r * cosV) * cosU;
            float y = (R + r * cosV) * sinU;
            float z = r * sinV;
            Torus::vertices[i * vSteps + j] = { x, y, z };
        }
    }
}

void Torus::loadFaces(int uSteps, int vSteps) {
    // Each quad gives rise to 2 triangles.
    // Total faces = uSteps * vSteps * 2
    int faceIndex = 0;
    for (int i = 0; i < uSteps; i++) {
        int nextI = (i + 1) % uSteps;
        for (int j = 0; j < vSteps; j++) {
            int nextJ = (j + 1) % vSteps;
            // Calculate indices for the current quad
            int idx0 = i * vSteps + j;
            int idx1 = nextI * vSteps + j;
            int idx2 = nextI * vSteps + nextJ;
            int idx3 = i * vSteps + nextJ;
            Torus::faces[faceIndex].color   = 0xff0058fc;
            Torus::faces[faceIndex].vertex1 = idx2;
            Torus::faces[faceIndex].vertex2 = idx1; // wrap-around for the quad
            Torus::faces[faceIndex].vertex3 = idx0;
            faceIndex++;
            Torus::faces[faceIndex].color   = 0Xffffffff;
            Torus::faces[faceIndex].vertex1 = idx3;
            Torus::faces[faceIndex].vertex2 = idx2; // wrap-around for the quad
            Torus::faces[faceIndex].vertex3 = idx0;
            faceIndex++;

        }
    }
}