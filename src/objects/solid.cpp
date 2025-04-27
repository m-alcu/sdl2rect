#include <iostream>
#include <math.h>
#include "../rasterizer.hpp"
#include "../smath.hpp"

void Solid::calculateNormals() {

    for (int i = 0; i < numFaces; i++) {
        const Face &face = Solid::faceData[i].faces;
        slib::vec3 v1 = Solid::vertexData[face.vertex1].vertices;
        slib::vec3 v2 = Solid::vertexData[face.vertex2].vertices;
        slib::vec3 v3 = Solid::vertexData[face.vertex3].vertices;

        // Calculate the edge vectors.
        slib::vec3 v21 = v2 - v1;
        slib::vec3 v32 = v3 - v2;

        Solid::faceData[i].faceNormals = smath::normalize(smath::cross(v21, v32));
    }
}

void Solid::calculateVertexNormals() {

    for (int i = 0; i < numVertices; i++) { 
        slib::vec3 vertexNormal = { 0, 0, 0 };
        for(int j = 0; j < numFaces; j++) {
            if (Solid::faceData[j].faces.vertex1 == i || 
                Solid::faceData[j].faces.vertex2 == i || 
                Solid::faceData[j].faces.vertex3 == i) {
                    vertexNormal += Solid::faceData[j].faceNormals;
            }
        }
        Solid::vertexData[i].vertexNormals = smath::normalize(vertexNormal);
    }

}

// Function returning MaterialProperties struct
MaterialProperties Solid::getMaterialProperties(MaterialType type) {
    switch (type) {
        case MaterialType::Rubber:   return {0.1f, 0.2f, 0.5f, 2};  // Low specular, moderate ambient, height diffuse
        case MaterialType::Plastic:  return {0.3f, 0.2f, 0.6f, 2};
        case MaterialType::Wood:     return {0.2f, 0.3f, 0.7f, 2};
        case MaterialType::Marble:   return {0.4f, 0.4f, 0.8f, 2};
        case MaterialType::Glass:    return {0.6f, 0.1f, 0.2f, 2};  // High specular, low ambient, low diffuse
        case MaterialType::Metal:    return {0.4f, 0.2f, 0.4f, 30}; // Almost no diffuse, very reflective
        case MaterialType::Mirror:   return {1.0f, 0.0f, 0.0f, 2};  // Perfect specular reflection, no ambient or diffuse
        default: return {0.0f, 0.0f, 0.0f, 0};
    }
}

int Solid::getColorFromMaterial(const float color) {

    float kaR = std::fmod(color, 1.0f);
    kaR = kaR < 0 ? 1.0f + kaR : kaR;
    return (static_cast<int>(kaR * 255));
}



