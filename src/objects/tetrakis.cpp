#include <iostream>
#include <math.h>
#include <cstdint>
#include "tetrakis.hpp"

void Tetrakis::loadVertices() {
    const float half = 50.f;
    const float axisDist = half * std::sqrt(3.f);  

    std::vector<VertexData> vertices;

    int index = 0;
    // Generate the 8 cube vertices with explicit sign choices
    for (int xSign : {1, -1}) {
        for (int ySign : {1, -1}) {
            for (int zSign : {1, -1}) {
                vertices.push_back({ half * xSign, half * ySign, half * zSign });
            }
        }
    }

    // Generate the 6 axis-aligned vertices

    vertices.push_back({ axisDist, 0, 0 });
    vertices.push_back({ 0, axisDist, 0 });
    vertices.push_back({ 0, 0, axisDist });
    vertices.push_back({ -axisDist, 0, 0 });
    vertices.push_back({ 0, -axisDist, 0 });
    vertices.push_back({ 0, 0, -axisDist });

    Tetrakis::vertexData = vertices;
    Tetrakis::numVertices = vertices.size();
}

void Tetrakis::loadFaces() {

    std::vector<FaceData> faces;

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
            uint32_t color = ((j % 2 == 0) ? 0xff0058fc : 0Xffffffff);

            FaceData face;
            face.face.vertex1 = quads[i][(j + 1) % 4]; // wrap-around for the quad
            face.face.vertex2 = quads[i][j];
            face.face.vertex3 = centers[i];

            MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

            if (j % 2 == 0) {
                face.face.material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
                face.face.material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc };
                face.face.material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
                face.face.material.Ns = properties.shininess;
            } else {
                face.face.material.Ka = { properties.k_a * 0xff, properties.k_a * 0xff, properties.k_a * 0xff };
                face.face.material.Kd = { properties.k_d * 0xff, properties.k_d * 0xff, properties.k_d * 0xff };
                face.face.material.Ks = { properties.k_s * 0xff, properties.k_s * 0xff, properties.k_s * 0xff };
                face.face.material.Ns = properties.shininess;
            }

            faces.push_back(face);
        }
    }

    Tetrakis::faceData = faces;
    Tetrakis::numFaces = faces.size();
}