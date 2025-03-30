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

    std::vector<Face> faces;
    faces.push_back({ 0+4, 1+4, 2+4, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    faces.push_back({ 0+4, 2+4, 3+4, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    faces.push_back({ 0, 1, 2, { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal)} });
    faces.push_back({ 0, 2, 3, { 0xffffffff, 0xffffffff, 0xffffffff, getMaterialProperties(MaterialType::Metal)} });
    this->faces = faces;
    this->numFaces = faces.size();

}