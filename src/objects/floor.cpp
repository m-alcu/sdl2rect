#include <iostream>
#include <math.h>
#include <cstdint>
#include "floor.hpp"


void Floor::loadVertices() {
    const float half = 10.f;

    std::vector<VertexData> vertices;
    vertices.push_back({   half,  half,  0 });
    vertices.push_back({  -half,  half,  0 });
    vertices.push_back({  -half, -half,  0 });
    vertices.push_back({   half, -half,  0 });

    this->vertexData = vertices;
    this->numVertices = vertices.size();
}

void Floor::loadFaces() {

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    std::vector<FaceData> faces;

    FaceData face;

    face.face.vertex1 = 0;
    face.face.vertex2 = 1;
    face.face.vertex3 = 2;
    face.face.material.Ka = { properties.k_a * 0x22, properties.k_a * 0x22, properties.k_a * 0x22 };
    face.face.material.Kd = { properties.k_d * 0x22, properties.k_d * 0x22, properties.k_d * 0x22 }; 
    face.face.material.Ks = { properties.k_s * 0x22, properties.k_s * 0x22, properties.k_s * 0x22 };
    face.face.material.Ns = properties.shininess;
    faces.push_back(face);

    face.face.vertex1 = 0;
    face.face.vertex2 = 2;
    face.face.vertex3 = 3;
    face.face.material.Ka = { properties.k_a * 0x22, properties.k_a * 0x22, properties.k_a * 0x22 };
    face.face.material.Kd = { properties.k_d * 0x22, properties.k_d * 0x22, properties.k_d * 0x22 };
    face.face.material.Ks = { properties.k_s * 0x22, properties.k_s * 0x22, properties.k_s * 0x22 };
    face.face.material.Ns = properties.shininess;
    faces.push_back(face);

    this->faceData = faces;
    this->numFaces = faces.size();

}