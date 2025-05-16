#include <iostream>
#include <math.h>
#include <cstdint>
#include "cube.hpp"


void Cube::loadVertices() {
    const float half = 10.f;

    std::vector<VertexData> vertices;
    VertexData v;

    // Front face (z = +half)
    v.vertex = { -half, -half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, -half, +half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    
    v.vertex = { -half, -half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { -half, +half, +half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    // Back face (z = -half)
    v.vertex = { +half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { -half, -half, -half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { -half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);

    v.vertex = { +half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { -half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { +half, +half, -half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    // Left face (x = -half)
    v.vertex = { -half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { -half, -half, +half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { -half, +half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);

    v.vertex = { -half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { -half, +half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { -half, +half, -half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    // Right face (x = +half)
    v.vertex = { +half, -half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, -half, -half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);

    v.vertex = { +half, -half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { +half, +half, +half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    // Top face (y = +half)
    v.vertex = { -half, +half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, +half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);

    v.vertex = { -half, +half, +half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, +half, -half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { -half, +half, -half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    // Bottom face (y = -half)
    v.vertex = { -half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, -half, -half }; v.texCoord = { 1, 1 }; vertices.push_back(v);
    v.vertex = { +half, -half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);

    v.vertex = { -half, -half, -half }; v.texCoord = { 0, 1 }; vertices.push_back(v);
    v.vertex = { +half, -half, +half }; v.texCoord = { 1, 0 }; vertices.push_back(v);
    v.vertex = { -half, -half, +half }; v.texCoord = { 0, 0 }; vertices.push_back(v);

    this->vertexData = vertices;
    this->numVertices = vertices.size();
}

void Cube::loadFaces() {
    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    std::string materialKey = "floorTexture";
    std::string mtlPath = "checker-map_tho.png";

    // Create and store the material
    slib::material material{};
    material.map_Kd = DecodePng(std::string(RES_PATH + mtlPath).c_str());
    materials.insert({materialKey, material});

    // Each face has 2 triangles, so for each face we generate 6 indices
    for (int face = 0; face < 6; ++face) {
        int baseIndex = face * 6;

        FaceData face1 {
            .face = {
                static_cast<int16_t>(baseIndex + 0),
                static_cast<int16_t>(baseIndex + 1),
                static_cast<int16_t>(baseIndex + 2),
                materials.at(materialKey),
                materialKey
            }
        };

        FaceData face2 {
            .face = {
                static_cast<int16_t>(baseIndex + 3),
                static_cast<int16_t>(baseIndex + 4),
                static_cast<int16_t>(baseIndex + 5),
                materials.at(materialKey),
                materialKey
            }
        };

        this->faceData.push_back(face1);
        this->faceData.push_back(face2);
    }

    this->numFaces =this->faceData.size();
}
