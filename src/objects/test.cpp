#include <iostream>
#include <math.h>
#include <cstdint>
#include "test.hpp"


void Test::loadVertices() {
    const float half = 10.f;
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

    FaceData face;
    std::vector<FaceData> faces;

    // Create and store the material
    slib::material material{};
    material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
    material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc }; 
    material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
    material.Ns = properties.shininess;
    materials.insert({"blue", material});

    material.Ka = { properties.k_a * 0xff, properties.k_a * 0xff, properties.k_a * 0xff };
    material.Kd = { properties.k_d * 0xff, properties.k_d * 0xff, properties.k_d * 0xff };
    material.Ks = { properties.k_s * 0xff, properties.k_s * 0xff, properties.k_s * 0xff };
    material.Ns = properties.shininess;
    materials.insert({"white", material});

    face.face.vertex1 = 0+4;
    face.face.vertex2 = 1+4;
    face.face.vertex3 = 2+4;
    face.face.materialKey = "blue";
    faces.push_back(face);

    face.face.vertex1 = 0+4;
    face.face.vertex2 = 2+4;
    face.face.vertex3 = 3+4;
    face.face.materialKey = "white";
    faces.push_back(face);

    face.face.vertex1 = 0;
    face.face.vertex2 = 1;
    face.face.vertex3 = 2;
    face.face.materialKey = "blue";
    faces.push_back(face);

    face.face.vertex1 = 0;
    face.face.vertex2 = 2;
    face.face.vertex3 = 3;
    face.face.materialKey = "white";
    faces.push_back(face);

    this->faceData = faces;
    this->numFaces = faces.size();

}