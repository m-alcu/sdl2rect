#include <iostream>
#include <math.h>
#include "../poly.hpp"

void Solid::calculateNormals() {
    for (int i = 0; i < numFaces; i++) {
        const Face &face = Solid::faces[i];
        Vec3 v1 = Solid::vertices[face.vertex1];
        Vec3 v2 = Solid::vertices[face.vertex2];
        Vec3 v3 = Solid::vertices[face.vertex3];

        // Calculate the edge vectors.
        Vec3 v21 = v2 - v1;
        Vec3 v32 = v3 - v2;

        // Compute the face normal via the cross product and normalize it.
        Solid::faceNormals[i] = v21.cross(v32).normalize();
    }
}

void Solid::calculateVertexNormals() {

    for (int i = 0; i < numVertices; i++) { 
        Vec3 vertexNormal = { 0, 0, 0 };
        for(int j = 0; j < numFaces; j++) {
            if (Solid::faces[j].vertex1 == i || 
                Solid::faces[j].vertex2 == i || 
                Solid::faces[j].vertex3 == i) {
                    vertexNormal += Solid::faceNormals[j];
            }
        }
        Solid::vertexNormals[i] = vertexNormal.normalize();
    }
}

// Function returning MaterialProperties struct
MaterialProperties Solid::getMaterialProperties(MaterialType type) {
    switch (type) {
        case MaterialType::Rubber:   return {0.1f, 0.2f, 0.5f, 2};  // Low specular, moderate ambient, high diffuse
        case MaterialType::Plastic:  return {0.3f, 0.2f, 0.6f, 2};
        case MaterialType::Wood:     return {0.2f, 0.3f, 0.7f, 2};
        case MaterialType::Marble:   return {0.4f, 0.4f, 0.8f, 2};
        case MaterialType::Glass:    return {0.6f, 0.1f, 0.2f, 2};  // High specular, low ambient, low diffuse
        case MaterialType::Metal:    return {0.9f, 0.05f, 0.1f, 2}; // Almost no diffuse, very reflective
        case MaterialType::Mirror:   return {1.0f, 0.0f, 0.0f, 2};  // Perfect specular reflection, no ambient or diffuse
        default: return {0.0f, 0.0f, 0.0f, 0};
    }
}



