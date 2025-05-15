#include <iostream>
#include <math.h>
#include <cstdint>
#include "floor.hpp"


void Floor::loadVertices() {
    const float half = 10.f;

    std::vector<VertexData> vertices;

    VertexData vertexData;
    vertexData.vertex = {  half,  half,  0 };
    vertexData.texCoord = { 0, 0 };

    vertices.push_back(vertexData);

    vertexData.vertex = { -half,  half,  0 };
    vertexData.texCoord = { 1, 0 };
    vertices.push_back(vertexData);

    vertexData.vertex = { -half, -half,  0 };
    vertexData.texCoord = { 1, 1 };
    vertices.push_back(vertexData);

    vertexData.vertex = {  half, -half,  0 };
    vertexData.texCoord = { 0, 1 };
    vertices.push_back(vertexData);

    this->vertexData = vertices;
    this->numVertices = vertices.size();
}

void Floor::loadFaces() {

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    std::vector<FaceData> faces;

    std::string materialKey = "floorTexture";
    std::string mtlPath = "checker-map_tho.png";

    // Create the material
    slib::material material{};
    material.map_Kd = DecodePng(std::string(RES_PATH + mtlPath).c_str());
    materials.insert({materialKey, material});


    FaceData face1 {
        .face = {0, 1, 2, materials.at(materialKey), materialKey},
        .faceNormal = {0, 0, 1}
    };

    FaceData face2 {
        .face = {0, 2, 3, materials.at(materialKey), materialKey},
        .faceNormal = {0, 0, 1}
    };

    this->faceData.push_back(face1);
    this->faceData.push_back(face2);
    this->numFaces = faces.size();

}