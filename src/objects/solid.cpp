#include <iostream>
#include <math.h>
#include "../rasterizer.hpp"
#include "../smath.hpp"

void Solid::calculateNormals() {

    // Calculate face normals
    std::vector<slib::vec3> faceNormals;

    for (int i = 0; i < numFaces; i++) {
        const Face &face = Solid::faces[i];
        slib::vec3 v1 = Solid::vertices[face.vertex1];
        slib::vec3 v2 = Solid::vertices[face.vertex2];
        slib::vec3 v3 = Solid::vertices[face.vertex3];

        // Calculate the edge vectors.
        slib::vec3 v21 = v2 - v1;
        slib::vec3 v32 = v3 - v2;

        faceNormals.push_back(smath::normalize(smath::cross(v21, v32)));
    }

    Solid::faceNormals = faceNormals;

}

void Solid::calculateVertexNormals() {

    std::vector<slib::vec3> vertexNormals;

    for (int i = 0; i < numVertices; i++) { 
        slib::vec3 vertexNormal = { 0, 0, 0 };
        for(int j = 0; j < numFaces; j++) {
            if (Solid::faces[j].vertex1 == i || 
                Solid::faces[j].vertex2 == i || 
                Solid::faces[j].vertex3 == i) {
                    vertexNormal += Solid::faceNormals[j];
            }
        }
        // Normalize the vertex normal
        vertexNormals.push_back(smath::normalize(vertexNormal));
    }

    Solid::vertexNormals = vertexNormals;
}

// Function returning MaterialProperties struct
MaterialProperties Solid::getMaterialProperties(MaterialType type) {
    switch (type) {
        case MaterialType::Rubber:   return {0.1f, 0.2f, 0.5f, 2};  // Low specular, moderate ambient, height diffuse
        case MaterialType::Plastic:  return {0.3f, 0.2f, 0.6f, 2};
        case MaterialType::Wood:     return {0.2f, 0.3f, 0.7f, 2};
        case MaterialType::Marble:   return {0.4f, 0.4f, 0.8f, 2};
        case MaterialType::Glass:    return {0.6f, 0.1f, 0.2f, 2};  // High specular, low ambient, low diffuse
        case MaterialType::Metal:    return {0.9f, 0.05f, 0.1f, 2}; // Almost no diffuse, very reflective
        case MaterialType::Mirror:   return {1.0f, 0.0f, 0.0f, 2};  // Perfect specular reflection, no ambient or diffuse
        default: return {0.0f, 0.0f, 0.0f, 0};
    }
}



