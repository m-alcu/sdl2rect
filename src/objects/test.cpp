#include <iostream>
#include <math.h>
#include "../rasterizer.hpp"


void Test::loadVertices() {
    const float half = 32768.f;
    const float axisDist = half * std::sqrt(3.f);  // ≈ 56755

    std::vector<VertexData> vertices;
    vertices.push_back({   axisDist,  axisDist,  axisDist });
    vertices.push_back({  -axisDist,  axisDist,  axisDist });
    vertices.push_back({  -axisDist, -axisDist,  axisDist });
    vertices.push_back({   axisDist, -axisDist,  axisDist });

    vertices.push_back({   axisDist,  axisDist, -axisDist });
    vertices.push_back({  -axisDist,  axisDist, -axisDist });
    vertices.push_back({  -axisDist, -axisDist, -axisDist });
    vertices.push_back({   axisDist, -axisDist, -axisDist });
    this->vertexData = vertices;
    this->numVertices = vertices.size();
}

void Test::loadFaces() {

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    std::vector<FaceData> faces;

    FaceData face;

    face.faces.vertex1 = 0+4;
    face.faces.vertex2 = 1+4;
    face.faces.vertex3 = 2+4;
    face.faces.material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
    face.faces.material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc }; 
    face.faces.material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
    face.faces.material.Ns = properties.shininess;
    faces.push_back(face);

    face.faces.vertex1 = 0+4;
    face.faces.vertex2 = 2+4;
    face.faces.vertex3 = 3+4;
    face.faces.material.Ka = { properties.k_a * 0xff, properties.k_a * 0xff, properties.k_a * 0xff };
    face.faces.material.Kd = { properties.k_d * 0xff, properties.k_d * 0xff, properties.k_d * 0xff };
    face.faces.material.Ks = { properties.k_s * 0xff, properties.k_s * 0xff, properties.k_s * 0xff };
    face.faces.material.Ns = properties.shininess;
    faces.push_back(face);

    face.faces.vertex1 = 0;
    face.faces.vertex2 = 1;
    face.faces.vertex3 = 2;
    face.faces.material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
    face.faces.material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc };
    face.faces.material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
    face.faces.material.Ns = properties.shininess;
    faces.push_back(face);

    face.faces.vertex1 = 0;
    face.faces.vertex2 = 2;
    face.faces.vertex3 = 3;
    face.faces.material.Ka = { properties.k_a * 0xff, properties.k_a * 0xff, properties.k_a * 0xff };
    face.faces.material.Kd = { properties.k_d * 0xff, properties.k_d * 0xff, properties.k_d * 0xff };
    face.faces.material.Ks = { properties.k_s * 0xff, properties.k_s * 0xff, properties.k_s * 0xff };
    face.faces.material.Ns = properties.shininess;
    faces.push_back(face);

    //faces.push_back({ 0+4, 1+4, 2+4, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0+4, 2+4, 3+4, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0, 1, 2, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0, 2, 3, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    this->faceData = faces;
    this->numFaces = faces.size();

}