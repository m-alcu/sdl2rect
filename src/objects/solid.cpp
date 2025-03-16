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

void Solid::calculateVec3Normals() {

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

void Solid::calculatePrecomputedShading(Scene scene) {

    MaterialProperties material = Solid::getMaterialProperties(MaterialType::Metal);

    for(int x = 0; x < 1024; x++) {
        for (int y = 0; y < 1024; y++) {

            float f_x = (float)(x - 512) / 512.0f;
            float f_y = (float)(y - 512) / 512.0f;

            float f_xy_sq = f_x * f_x + f_y * f_y;

            if (f_xy_sq <= 1.0f) {
                float f_z = sqrt(1.0f - f_xy_sq);

                Vec3 normal = Vec3(f_x, f_y, f_z).normalize();
                float diff = std::max(0.0f, normal.dot(scene.lux));
            
                Vec3 R = (normal * 2.0f * normal.dot(scene.lux) - scene.lux).normalize();
                float specAngle = std::max(0.0f, R.dot(scene.lux)); // viewer
                float spec = std::pow(specAngle, material.shininess);
            
                float bright = material.k_a+material.k_d * diff+ material.k_s * spec;
                Solid::precomputedShading[y*1024+x] = (int32_t) (bright * 65536 * 0.98);
            } else {
                Solid::precomputedShading[y*1024+x] = 0;
            }
        }
    }

}

