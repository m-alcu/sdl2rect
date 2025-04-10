#include <iostream>
#include <math.h>
#include "../rasterizer.hpp"


void Test::loadVertices() {
    const float half = 32768.f;
    const float axisDist = half * std::sqrt(3.f);  // ≈ 56755

    std::vector<slib::vec3> vertices;
    vertices.push_back({   axisDist,  axisDist,  axisDist });
    vertices.push_back({  -axisDist,  axisDist,  axisDist });
    vertices.push_back({  -axisDist, -axisDist,  axisDist });
    vertices.push_back({   axisDist, -axisDist,  axisDist });

    vertices.push_back({   axisDist,  axisDist, -axisDist });
    vertices.push_back({  -axisDist,  axisDist, -axisDist });
    vertices.push_back({  -axisDist, -axisDist, -axisDist });
    vertices.push_back({   axisDist, -axisDist, -axisDist });
    this->vertices = vertices;
    this->numVertices = vertices.size();
}

void Test::loadFaces() {

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    std::vector<Face> faces;

    Face face;

    face.vertex1 = 0+4;
    face.vertex2 = 1+4;
    face.vertex3 = 2+4;
    face.material.Ka = { getColorFromMaterial(properties.k_a * 0x00), getColorFromMaterial(properties.k_a * 0x58), getColorFromMaterial(properties.k_a * 0xfc) };
    face.material.Kd = { getColorFromMaterial(properties.k_d * 0x00), getColorFromMaterial(properties.k_d * 0x58), getColorFromMaterial(properties.k_d * 0xfc) }; 
    face.material.Ks = { getColorFromMaterial(properties.k_s * 0x00), getColorFromMaterial(properties.k_s * 0x58), getColorFromMaterial(properties.k_s * 0xfc) };
    face.material.Ns = properties.shininess;
    faces.push_back(face);

    face.vertex1 = 0+4;
    face.vertex2 = 2+4;
    face.vertex3 = 3+4;
    face.material.Ka = { getColorFromMaterial(properties.k_a * 0xff), getColorFromMaterial(properties.k_a * 0xff), getColorFromMaterial(properties.k_a * 0xff) };
    face.material.Kd = { getColorFromMaterial(properties.k_d * 0xff), getColorFromMaterial(properties.k_d * 0xff), getColorFromMaterial(properties.k_d * 0xff) };
    face.material.Ks = { getColorFromMaterial(properties.k_s * 0xff), getColorFromMaterial(properties.k_s * 0xff), getColorFromMaterial(properties.k_s * 0xff) };
    face.material.Ns = properties.shininess;
    faces.push_back(face);

    face.vertex1 = 0;
    face.vertex2 = 1;
    face.vertex3 = 2;
    face.material.Ka = { getColorFromMaterial(properties.k_a * 0x00), getColorFromMaterial(properties.k_a * 0x58), getColorFromMaterial(properties.k_a * 0xfc) };
    face.material.Kd = { getColorFromMaterial(properties.k_d * 0x00), getColorFromMaterial(properties.k_d * 0x58), getColorFromMaterial(properties.k_d * 0xfc) };
    face.material.Ks = { getColorFromMaterial(properties.k_s * 0x00), getColorFromMaterial(properties.k_s * 0x58), getColorFromMaterial(properties.k_s * 0xfc) };
    face.material.Ns = properties.shininess;
    faces.push_back(face);

    face.vertex1 = 0;
    face.vertex2 = 2;
    face.vertex3 = 3;
    face.material.Ka = { getColorFromMaterial(properties.k_a * 0xff), getColorFromMaterial(properties.k_a * 0xff), getColorFromMaterial(properties.k_a * 0xff) };
    face.material.Kd = { getColorFromMaterial(properties.k_d * 0xff), getColorFromMaterial(properties.k_d * 0xff), getColorFromMaterial(properties.k_d * 0xff) };
    face.material.Ks = { getColorFromMaterial(properties.k_s * 0xff), getColorFromMaterial(properties.k_s * 0xff), getColorFromMaterial(properties.k_s * 0xff) };
    face.material.Ns = properties.shininess;
    faces.push_back(face);

    //faces.push_back({ 0+4, 1+4, 2+4, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0+4, 2+4, 3+4, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0, 1, 2, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    //faces.push_back({ 0, 2, 3, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    this->faces = faces;
    this->numFaces = faces.size();

}