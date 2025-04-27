#include <iostream>
#include <math.h>
#include "../rasterizer.hpp"
#include "../constants.hpp"

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

    std::vector<VertexData> vertices;
    vertices.resize(uSteps * vSteps);
    Torus::vertexData = vertices;

    for (int i = 0; i < uSteps; i++) {
        float u = i * 2 * PI / uSteps;
        float cosU = cos(u);
        float sinU = sin(u);
        for (int j = 0; j < vSteps; j++) {
            float v = j * 2 * PI / vSteps;
            float cosV = cos(v);
            float sinV = sin(v);
            float x = (R + r * cosV) * cosU;
            float y = (R + r * cosV) * sinU;
            float z = r * sinV;
            Torus::vertexData[i * vSteps + j].vertex = { x, y, z };
        }
    }

    Torus::numVertices = vertices.size();
}

void Torus::loadFaces(int uSteps, int vSteps) {
    // Each quad gives rise to 2 triangles.
    // Total faces = uSteps * vSteps * 2

    std::vector<FaceData> faces;

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

            FaceData face;
            face.face.vertex1 = idx0;
            face.face.vertex2 = idx1; // wrap-around for the quad
            face.face.vertex3 = idx2;
            MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

            face.face.material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
            face.face.material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc };
            face.face.material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
            face.face.material.Ns = properties.shininess;
            faces.push_back(face);

            face.face.vertex1 = idx0;
            face.face.vertex2 = idx2; // wrap-around for the quad
            face.face.vertex3 = idx3;
            face.face.material.Ka = { properties.k_a * 0xfe, properties.k_a * 0xdd, properties.k_a * 0x00 };
            face.face.material.Kd = { properties.k_d * 0xfe, properties.k_d * 0xdd, properties.k_d * 0x00 };
            face.face.material.Ks = { properties.k_s * 0xfe, properties.k_s * 0xdd, properties.k_s * 0x00 };
            face.face.material.Ns = properties.shininess;
            faces.push_back(face);
        }
    }

    Torus::faceData = faces;
    Torus::numFaces = faces.size();
}